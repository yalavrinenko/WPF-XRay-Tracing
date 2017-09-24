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
    public class DetectorPlot
    {
        private PlotView m_Canvas;
        private Plot m_Spectrum;


        public DetectorPlot(PlotView canvas, Plot spectrumCanvas)
        {
            m_Canvas = canvas;
            m_Spectrum = spectrumCanvas;
        }

        public void PlotDetector(Detector detector)
        {
            var model = new PlotModel();

            var sser = new OxyPlot.Series.HeatMapSeries()
            {
                X0 = detector.MinDetectorPositionMer,
                X1 = detector.MaxDetectorPositionMer,
                Y0 = detector.MinDetectorPositionSag - detector.PixelSize / 2.0,
                Y1 = detector.MaxDetectorPositionSag + detector.PixelSize / 2.0,
                Data = detector.GetHeatMap(),
                Interpolate = false,
                RenderMethod = HeatMapRenderMethod.Bitmap
            };

            model.Series.Add(sser);
            model.Axes.Add(new OxyPlot.Axes.LinearAxis { Title = "x, [mm]", Position = AxisPosition.Bottom, Maximum = detector.MaxDetectorPositionMer, Minimum = detector.MinDetectorPositionMer });
            model.Axes.Add(new OxyPlot.Axes.LinearAxis { Title = "y, [mm]", Position = AxisPosition.Left, Maximum = detector.MaxDetectorPositionSag, Minimum = detector.MinDetectorPositionSag });
            model.Axes.Add(new OxyPlot.Axes.LinearColorAxis { Position = AxisPosition.Right, Palette = OxyPalettes.Gray(1000).Reverse()});

           

            model.Axes[0].AxisChanged += DetectorPlotX_AxisChanged;
            model.Axes[1].AxisChanged += DetectorPlotY_AxisChanged;

            m_Canvas.Model = model;
            m_Canvas.InvalidatePlot(true);
        }

        public void PlotSpectrum(Detector detector, double OrderScale = 1)
        {
            if (detector == null)
                return;

            var bins = detector.GetHistogram();

            if (bins == null)
                return;

            m_Spectrum.Series.Clear();
            m_Spectrum.InvalidatePlot(true);

            var lineSeries = new OxyPlot.Wpf.LineSeries()
            {
                LineStyle = LineStyle.Solid,
                Color = OxyColors.RoyalBlue.ToColor(),
                BrokenLineColor = OxyColors.RoyalBlue.ToColor()
            };
            lineSeries.YAxisKey = "Counts";
            lineSeries.XAxisKey = "XPosition";

            var step = (Math.Abs(detector.MeredionalMax - detector.MeredionalMin) / bins.Length);

            List<DataPoint> pointList = new List<DataPoint>();
            for (int i = 0; i < bins.Length; ++i)
            {
                double x = detector.MeredionalMin + i * step - detector.XShift;
                pointList.Add(new DataPoint(x, bins[i]));
            }
            lineSeries.ItemsSource = pointList;
            lineSeries.Items.Refresh();


            var waveSeries = new OxyPlot.Wpf.LineSeries()
            {
                LineStyle = LineStyle.None,
                Color = OxyColors.RoyalBlue.ToColor(),
                BrokenLineColor = OxyColors.RoyalBlue.ToColor()
            };
            waveSeries.YAxisKey = "Counts";
            waveSeries.XAxisKey = "WaveLength";

            List<DataPoint> waveList = new List<DataPoint>();
            if (detector.ZeroCurve != null)
            {
                for (int i = 0; i < bins.Length; ++i)
                {
                    double x = detector.MeredionalMin + i * step - detector.XShift;
                    waveList.Add(new DataPoint(detector.ZeroCurve.Evaluate(x) * OrderScale, 1));
                }
            }
            waveSeries.ItemsSource = waveList;
            waveSeries.Items.Refresh();

            m_Spectrum.Series.Add(waveSeries);
            m_Spectrum.Series.Add(lineSeries);
            m_Spectrum.InvalidatePlot(true);

        }

        public void UpdateAxis(Detector detector)
        {
            m_Canvas.Model.Axes[0].Minimum = detector.MinDetectorPositionMer;
            m_Canvas.Model.Axes[1].Minimum = detector.MinDetectorPositionSag;

            m_Canvas.Model.Axes[0].Maximum = detector.MaxDetectorPositionMer;
            m_Canvas.Model.Axes[1].Maximum = detector.MaxDetectorPositionSag;

            m_Canvas.InvalidatePlot(true);
        }

        public struct Point
        {
            public double min;
            public double max;
        }

        public event EventHandler<Point> XZoom;
        public event EventHandler<Point> YZoom;

        private void DetectorPlotX_AxisChanged(object sender, AxisChangedEventArgs e)
        {
            var axis = sender as OxyPlot.Axes.LinearAxis;
            XZoom?.Invoke(this, new Point()
            {
                min = axis.ActualMinimum,
                max = axis.ActualMaximum
            });
        }

        private void DetectorPlotY_AxisChanged(object sender, AxisChangedEventArgs e)
        {
            var axis = sender as OxyPlot.Axes.LinearAxis;
            YZoom?.Invoke(this, new Point()
            {
                min = axis.ActualMinimum,
                max = axis.ActualMaximum
            });
        }
    }
}
