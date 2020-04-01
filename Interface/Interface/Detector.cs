using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OxyPlot;
using System.IO;
using OxyPlot.Series;
using OxyPlot.Wpf;
using OxyPlot.Axes;

namespace Interface
{
    public class Detector
    {
        public class DetectorPoint
        {
            public double x, y, z;
            public double lambda;
        }

        public class LineFeatures
        {
            public double XCoord;
            public double Size;
            public double FWHM;
            public DispersionCurve Curve; 
        }

        private List<DetectorPoint> m_Points;

        public class DispersionCurve
        {
            public double a = 0;
            public double b = 0;
            public double c = 0;
            public bool isExist = false;

            public double Evaluate(double x)
            {
                return a * x * x + b * x + c;
            }
            public override String ToString()
            {
                return (isExist) ? String.Format("Wavelength(x) = {0}*X^2 + {1}*X + {2}", Extension.mstr(a), Extension.mstr(b), Extension.mstr(c)): "Unable to construct curve";
            }
        }
#region Geometry
        public double MinDetectorPositionSag { get; set; } = 0;
        public double MaxDetectorPositionSag { get; set; } = 0;

        public double MinDetectorPositionMer { get; set; } = 0;
        public double MaxDetectorPositionMer { get; set; } = 0;

        public double PixelSize { get; set; } = 0.025;

        public double MeredionalMax;
        public double MeredionalMin;

        public double SaggitalMax;
        public double SaggitalMin;
        #endregion

        #region zero wave

        public double XShift = 0;
        public DispersionCurve ZeroCurve = null;

        #endregion

        public bool isReady = false;

        public double WaveOrderScale = 1;

        private long[] m_Histogram;
        private double m_current_histogram_step;
        private double[,] m_HeatMap;

        public List<DetectorPoint> Points { get { return m_Points; } }

        public Detector()
        {
        }

        public void BuildHistogram()
        {
            var range = new Extension.Range()
            {
                VMax = this.MeredionalMax,
                VMin = this.MeredionalMin
            };

            long NBINS = 1000;

            double step = (Math.Abs(range.VMax - range.VMin) / NBINS);

            m_Histogram = Extension.Histogram(this.Points.Select(p => p.x), range, this.PixelSize);
            m_current_histogram_step = this.PixelSize;
        }

        public void BuildHeatMap()
        {
            var xrange = new Extension.Range()
            {
                VMax = this.MaxDetectorPositionMer,
                VMin = this.MinDetectorPositionMer
            };

            var yrange = new Extension.Range()
            {
                VMax = this.MaxDetectorPositionSag,
                VMin = this.MinDetectorPositionSag
            };

            m_HeatMap = Extension.Histogram2D(this.Points.Select(p => p.x), Points.Select(p => p.z), xrange, yrange, PixelSize);
        }

        public double[,] GetHeatMap()
        {
            return m_HeatMap;
        }

        public long[] GetHistogram()
        {
            if (m_current_histogram_step != this.PixelSize)
                BuildHistogram();
            return m_Histogram;
        }

        public void ReadDetectorPlane(string fileName)
        {
            var lines = File.ReadLines(fileName);
            m_Points = new List<DetectorPoint>();

            try
            {
                var ScAngle = lines.ElementAt(1).Split('\t')[2];
                double phi = Double.Parse(ScAngle);

                var CoordLine = lines.ElementAt(2).Replace(";","").Replace("\t","").Replace(" ","").Replace("]","").Replace("[","").Split('=')[1].Split(',');
                double cx = Double.Parse(CoordLine[0]);
                double cy = Double.Parse(CoordLine[1]);
                double cz = Double.Parse(CoordLine[2]);

                var NormalLine = lines.ElementAt(3).Replace(";", "").Replace("\t", "").Replace(" ", "").Replace("]", "").Replace("[", "").Split('=')[1].Split(',');
                double ncx = Double.Parse(NormalLine[0]);
                double ncy = Double.Parse(NormalLine[1]);
                double ncz = Double.Parse(NormalLine[2]);

                phi *= Math.PI / 180.0;
                phi = (Math.PI / 2.0 - phi);

                if (cx > 0.0)
                    phi = -phi;

                Func<DetectorPoint, double, DetectorPoint> Rotate = (p, angle) => {
                    var wx = p.x;
                    var wy = p.y;
                    var dwx = wx * Math.Cos(angle) - wy * Math.Sin(angle);
                    var dwy = wx * Math.Sin(angle) + wy * Math.Cos(angle);
                    p.x = -dwx;
                    p.y = dwy;
                    return p;
                };

                m_Points = new List<DetectorPoint>();
                foreach (var line in lines.Skip(6))
                {
                    var coords = line.Split('\t');
                    DetectorPoint cp = new DetectorPoint()
                    {
                        x = double.Parse(coords[0]) - cx,
                        y = double.Parse(coords[1]) - cy,
                        z = double.Parse(coords[2]) - cz,
                        lambda = double.Parse(coords[3]),
                    };

                    m_Points.Add(Rotate(cp, phi));
                }

                MaxDetectorPositionMer = 30;// Math.Round(m_Points.Max(x => x.x) * 1.2, 3);
                MinDetectorPositionMer = -30;// Math.Round(m_Points.Min(x => x.x) * 1.2, 3);

                MeredionalMax = MaxDetectorPositionMer;
                MeredionalMin = MinDetectorPositionMer;

                MaxDetectorPositionSag = 5;// Math.Round(m_Points.Max(x => x.z) * 1.2, 3);
                MinDetectorPositionSag = -5;// Math.Round(m_Points.Min(x => x.z) * 1.2, 3);

                SaggitalMax = MaxDetectorPositionSag;
                SaggitalMin = MinDetectorPositionSag;

                isReady = true;
            }
            catch (Exception e)
            {
                Logger.Error(e.ToString());
                return;
            }
        }

        private DetectorPoint[] SelectPoints (Wave wave)
        {
            return m_Points.Where(w => wave.lambda - wave.dlambda * 1e-3 <= w.lambda && w.lambda <= wave.lambda + wave.dlambda * 1e-3).ToArray();
        }

        public LineFeatures GetLineFeatures(Wave wave)
        {
            var LinePoints = SelectPoints(wave);
            if (LinePoints.Length == 0)
                return null;

            var XCoord = LinePoints.Average(p => p.x);
            var FWHM = MathNet.Numerics.Statistics.Statistics.StandardDeviation(LinePoints.Select(p => p.x)) * 2.355;

            var Size = LinePoints.Max(p => p.z) - LinePoints.Min(p => p.z);

            var ShiftedSpectrum = m_Points.Select(p => new { x = p.x - XCoord, w = p.lambda });

            var Features = new LineFeatures()
            {
                XCoord = XCoord,
                Size = Size,
                FWHM = FWHM
            };
            try
            {
                var fitCoeff = MathNet.Numerics.Fit.Polynomial(ShiftedSpectrum.Select(shf => shf.x).ToArray(), ShiftedSpectrum.Select(shf => shf.w).ToArray(), 2);
                Features.Curve = new DispersionCurve()
                {
                    isExist = true,
                    a = fitCoeff[2],
                    b = fitCoeff[1],
                    c = fitCoeff[0]
                };
            }
            catch
            {
                Features.Curve = new DispersionCurve()
                {
                    isExist = false
                };
            }

            return Features; 
        }

        public void SetZeroWave(Wave wave)
        {
            var feature = GetLineFeatures(wave);
            XShift = feature.XCoord;
            ZeroCurve = wave.Curve;
        }
    }
}
