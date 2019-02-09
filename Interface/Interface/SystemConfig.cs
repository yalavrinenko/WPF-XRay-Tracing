using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet;
using MathNet.Numerics;
using System.IO;
using MathNet.Numerics.LinearAlgebra;

namespace Interface
{
    public class SystemConfig
    {
        public class ObjectInfo
        {
            public string GridType { get; set; } = "none";
            public short GridLocation { get; set; } = 0; //0 - before, 1 - after the mirror
            public double GridWidth { get; set; } = 10;
            public double GridHeight { get; set; } = 5;
            public double GridPixelSizeX { get; set; } = 0.001;
            public double GridPixelSizeY { get; set; } = 0.001;
            public string GridMap { get; set; } = "";
            public double GridPosition { get; set; } = 0.0;
        }

        public class CrystalInfo
        {
            public List<double> crystal2d { get; set; }
            public List<string> DistrFileName { get; set; }
            public List<double> DistrHW { get; set; }
            public List<double> DistrStep { get; set; }
            public List<double> DistrSize { get; set; }
            public List<uint> Orders { get; set; }

            public CrystalInfo()
            {
                crystal2d = new List<double>();
                DistrFileName = new List<string>();
                DistrHW = new List<double>();
                DistrStep = new List<double>();
                DistrSize = new List<double>();
                Orders = new List<uint>();
            }

            public void Add(uint order, double c2d, string fname, double hw, double step, double size)
            {
                Orders.Add(order);
                crystal2d.Add(c2d);
                DistrFileName.Add(fname);
                DistrHW.Add(hw);
                DistrStep.Add(step);
                DistrSize.Add(size);
            }
        }
        public CrystalInfo CrystalProps;

        #region public vars
        public string Crystal2d { get
            {
                var s = "";

                if (CrystalProps?.crystal2d == null)
                    return "2.45";

                foreach (var d in CrystalProps?.crystal2d)
                {
                    s += d.ToString() + " ";
                }
                return s;
            }
        }
        public double crystalR { get; set; } = 150;
        public double centralWave { get; set; } = 2.0;
        public double BraggA { get; set; } = 50;
        public double ScatterAngle
        {
            get { return 90 - BraggA; }
            set { BraggA = 90 - value; }
        }

        public double SrcDist { get; set; } = 100;
        public double DstDist { get; set; }
        public string CrystType { get; set; } = "Sphere"; // $MrType = "Sphere"  ;"Sphere" ;Cylinder;

        public double crystalW { get; set; } = 46;
        public double crystalH { get; set; } = 12;

        public double SrcSizeW { get; set; } = 0.1;
        public double SrcSizeH { get; set; } = 0.1;

        public double SrcCone { get; set; }
        public double SolidCone { get; set; }

        public double RayCount { get; set; } = 100000;
        public double RayByIter { get; set; } = 10000;

        public double FilmDistFromCenter { get; set; }
        public double ToFilmDirection { get; set; }
        public double FilmAngle { get; set; }
        public double FilmRotationAngle { get; set; }
        public double FilmSizeW { get; set; } = 1500;
        public double FilmSizeH { get; set; } = 1500;

        public int mainOrder { get; set; } = 1;

        public ObjectInfo Object { get; set; } = new ObjectInfo { };
        public bool ObjectExist { get; set; } = false;

        /*public double SlitPos { get; set; }
        public double SlitW { get; set; }
        public double SlitH { get; set; }*/

        public class WaveLimit
        {
            public double lambda { get; set; }
            public double position { get; set; }
        }

        public class WaveLimits
        {
            public WaveLimit min { get; set; }
            public WaveLimit max { get; set; }
        }

        public WaveLimits waveLimits { get; set; }

        public string MinWavePosStr
        {
            get
            {
                return "Position λmin = " + waveLimits?.min.position.ToString();
            }
        }

        public string MaxWavePosStr
        {
            get
            {
                return "Position λmax = " + waveLimits?.max.position.ToString();
            }
        }

        public string MinWaveStr { get
            {
                return "Min λ = " + waveLimits?.min.lambda.ToString();
            } set { }
        }

        public string MaxWaveStr
        {
            get
            {
                return "Max λ = " + waveLimits?.max.lambda.ToString();
            }
        }

        public double zeroWave { get; set; }

        public string ZeroWaveStr { get; set; }

        public bool UseBetterStatistic { get; set; } = false; ///TODO: Set false as default
        #endregion

        public event EventHandler onChange = null;
        public event EventHandler<WaveLimits> onWaveLimitChange = null;

        //public MainWindow m_MainForms { get; set; }

        public void ReadCrystalInfo(string path)
        {
            var crystStream = new System.IO.StreamReader(path);

            CrystalProps = new CrystalInfo();

            while (!crystStream.EndOfStream)
            {
                var lines = crystStream.ReadLine();
                if (!lines.Contains("#") && !lines.Contains("*"))
                {
                    try
                    {
                        var rline = lines.Trim();
                        rline = System.Text.RegularExpressions.Regex.Replace(rline, @"\s+", " ");

                        var sline = rline.Split(" \t".ToArray());
                        var order = uint.Parse(sline[0]);
                        var c2d = double.Parse(sline[1]);
                        var dpath = sline[2];

                        var info = GetCrystalInfo(dpath);
                        if (info != null)
                        {
                            CrystalProps.Add(order, c2d, dpath, info.Item1, info.Item2, info.Item3);
                        }
                    }
                    catch (Exception e)
                    {
                        Logger.Warning(e.Message);
                        continue;
                    }
                }
            }
            crystStream.Close();
            onChange?.Invoke(this, new EventArgs());
        }

        public string GenerateOutputConfig()
        {
            var data = "";
            data += "[INPUT]\n";
            data += "Crystal type = " + CrystType + "\n";
            try
            {
                data += "Crystal 2d = " + Extension.mstr(CrystalProps?.crystal2d[mainOrder - 1]) + " [A]\n";
            }
            catch(Exception e)
            {
                data += e.ToString();
            }
            data += "Crystal radius R = " + Extension.mstr(crystalR) + " [mm]\n";
            data += "Crystal W x H = " + Extension.mstr(crystalW) + " X " + Extension.mstr(crystalH) + " [mm]\n";
            data += "Source size W x H= " + Extension.mstr(SrcSizeW) + " X " + Extension.mstr(SrcSizeH) + " [mm]\n";
            data += "Detector size W x H= " + Extension.mstr(FilmSizeW) + " X " + Extension.mstr(FilmSizeH) + " [mm]\n";
            data += "Central wavelength = " + Extension.mstr(centralWave) + " [A]\n";
            data += "Source distance = " + Extension.mstr(SrcDist) + " [mm]\n";
            data += "Main reflection order m = " + Extension.mstr(mainOrder) + "\n";
            data += "Source solid angle = " + Extension.mstr(SolidCone) + " [sr]\n";

            data += "\n\n[OUTPUT]\n";
            data += "Bragg Angle = " + Extension.mstr(BraggA) + " [deg]\n";
            data += "Incident Angle = " + Extension.mstr(90.0 - BraggA) + " [deg]\n";
            data += "FRO angle = " + Extension.mstr(180.0 - ToFilmDirection) + " [deg]\n";
            data += "Detector angle = " + Extension.mstr(FilmRotationAngle) + " [deg]\n";
            data += "Detector to crystal = " + Extension.mstr(DstDist) + " [mm]\n";
            data += "Detector to center = " + Extension.mstr(FilmDistFromCenter) + " [mm]\n";

            if (ObjectExist)
            {
                data += "\n\n[OBJECT]\n";
                data += String.Format("Map = {0}\n", Object.GridMap);
                data += String.Format("Distance = {0} [mm]\n", Object.GridPosition);
                data += String.Format("Position = {0} crystal\n", (Object.GridLocation == 1) ? "after" : "before");
                data += String.Format("Object W x H = {0} X {1} [mm]\n", Object.GridWidth, Object.GridHeight);
                data += String.Format("PixelSizeX = {0} [mm]\n", Object.GridPixelSizeX);
                data += String.Format("PixelSizeY = {0} [mm]\n", Object.GridPixelSizeY);
            }

            return data;
        }

        public string GetCrystalConfig()
        {
            var data = "";
            for (int i=0; i<CrystalProps.crystal2d.Count; ++i) {
                data += "\n[REFL. FUNCTION IN " + (i + 1).ToString() + " ORDER]\n";
                data += "Crystal 2d = " + CrystalProps.crystal2d[i].ToString() + " [A]\n";
                data += "REFL. File Name = " + CrystalProps.DistrFileName[i] + "\n";
                data += "REFL. HW = " + (CrystalProps.DistrHW[i]).ToString() + " [deg]\n";
                data += "REFL. Step = " + (CrystalProps.DistrStep[i]).ToString() + "\n";
                data += "REFL. Size = " + (CrystalProps.DistrSize[i]).ToString() + " [points]\n";
             }
            return data;
        }

        public void ConstructFSSR1()
        {
            if (CrystalProps == null)
                throw new Exception("No crystal file");

            var sTheta = centralWave / (CrystalProps.crystal2d[mainOrder - 1] / mainOrder);
            if (sTheta > 1.0)
                throw new Exception("Bad central wave for selected order. Use wavelength less than " + Extension.mstr((CrystalProps.crystal2d[mainOrder - 1] / mainOrder)) + " [A]");

            BraggA = Math.Asin(sTheta) * 180.0 / Math.PI;

            var phi = (90.0 - BraggA) * Math.PI / 180.0;
            SrcDist = crystalR * Math.Cos(phi) / Math.Cos(2 * phi);
            DstDist = crystalR * Math.Cos(phi);

            MainGeometry(phi);
        }

        private void MainGeometry(double phi)
        {
            FilmDistFromCenter = Math.Sqrt(DstDist * DstDist + crystalR * crystalR / 4.0 - DstDist * crystalR * Math.Cos(phi));
            ToFilmDirection = Math.PI - Math.Asin(DstDist * Math.Sin(phi) / FilmDistFromCenter);
            ToFilmDirection *= 180.0 / Math.PI;

            var L = Math.Sqrt(SrcDist * SrcDist + DstDist * DstDist - 2 * SrcDist * DstDist * Math.Cos(2 * phi));
            var sGamma = SrcDist * Math.Sin(2 * phi) / L;
            var Gamma = Math.Asin(Math.Abs(sGamma) > 1.0 ? 1.0 : sGamma);
            var Gamma2 = Math.Asin(crystalR * Math.Sin(phi) / (2 * FilmDistFromCenter));
            FilmAngle = (Gamma - Gamma2) * 180.0 / Math.PI;
            GetRotationAngle(phi);

            var theta = BraggA * Math.PI / 180.0;

            var H = crystalR - Math.Sqrt(crystalR * crystalR - crystalW * crystalW / 4.0);
            var dh = H * Math.Tan(Math.PI / 2.0 - theta);
            var l = crystalW / 2.0 + dh;
            L = SrcDist - Math.Sqrt(dh * dh + H * H);

            var d = L * L + l * l - 2 * L * l * Math.Sin(Math.PI - theta);
            d = Math.Sqrt(d);

            var max_edge_dist = Math.Max(crystalH, crystalW);
            var cone_Cos = SrcDist / (Math.Sqrt(SrcDist * SrcDist + max_edge_dist * max_edge_dist));
            SrcCone = Math.Acos(cone_Cos);
            SrcCone *= 1.05;
            SolidCone = 2.0 * Math.PI * (1.0 - cone_Cos);
            SrcCone *= 180.0 / Math.PI;

            GetLimitWaves();

            onChange?.Invoke(this, new EventArgs());
        }

        public void UpdateConfiguration()
        {
            if (CrystalProps == null)
                throw new Exception("No crystal file");

            var phi = (90.0 - BraggA) * Math.PI / 180.0;
            DstDist = SrcDist * crystalR / (2.0 * SrcDist * Math.Cos(phi) - crystalR);

            MainGeometry(phi);
        }

        public void UpdateConfigurationWithNewCentralWave()
        {
            if (CrystalProps == null)
                throw new Exception("No crystal file");

            var sTheta = centralWave / (CrystalProps.crystal2d[mainOrder - 1] / mainOrder);
            if (sTheta > 1.0)
                throw new Exception("Bad central wave for selected order. Use wavelength less than " + Extension.mstr((CrystalProps.crystal2d[mainOrder - 1] / mainOrder)) + " [A]");

            BraggA = Math.Asin(sTheta) * 180.0 / Math.PI;

            UpdateConfiguration();
        }

        private void GetLimitWaves()
        {
            var phi = BraggA * Math.PI / 180.0;
            var L = SrcDist;
            var R = crystalR;
            var H = crystalW / 2.0;
            var D = DstDist;

            double[] CR = { 0.0, L * Math.Sin(phi) - R };
            double[] SR = { -L * Math.Cos(phi), 0 };

            var S = CR[1] + Math.Sqrt(R * R - H * H);

            double[] detector_center = { D * Math.Cos(phi), CR[1] + R - D * Math.Sin(phi) };

            var detector_norm_line = Extension.Sub(detector_center, SR);
            
            detector_norm_line = detector_norm_line.Select(d => d / Extension.vectorLenght(detector_norm_line)).ToArray();

            double[] detector_cross_line = { detector_norm_line[0], detector_norm_line[1] };

            List<double> waves = new List<double>();
            List<double> position = new List<double>();

            List<double[]> limit_line_cross_point = new List<double[]>();
            List<double> limit_line_position = new List<double>();

            foreach (var mult in new double[] { -1.0, 1.0 }) {
                double[] CrossR = { mult * H, S };
                var r1 = Extension.Sub(SR, CrossR);
                var r2 = Extension.Sub(CR, CrossR);
                var cosT = Extension.dot(r1, r2) / (Math.Sqrt(Extension.dot(r1, r1)) * Math.Sqrt(Extension.dot(r2, r2)));

                var Theta = Math.PI / 2.0 - Math.Acos(cosT);
                var P = (CrystalProps.crystal2d[mainOrder - 1] / mainOrder) * Math.Sin(Theta);
                waves.Add(P);

                Theta = Math.PI / 2 - Theta;
                var cos2T = Math.Cos(-Theta);
                var sin2T = Math.Sin(-Theta);

                double[,] rotation_matrix_vec = { { cos2T, -sin2T }, { sin2T, cos2T } };
                var rotation_matrix = Matrix<double>.Build.DenseOfArray(rotation_matrix_vec);

                r2 = r2.Select(x => x / Extension.vectorLenght(r2)).ToArray();

                var mirror_to_detector = Vector<double>.Build.DenseOfArray(r2) * rotation_matrix;
                mirror_to_detector.Divide(Extension.vectorLenght(mirror_to_detector.ToArray()));

                var A_array = new double[,] { { mirror_to_detector[0], mirror_to_detector[1]}, { detector_cross_line[0], detector_cross_line[1] } };
                var A = Matrix<double>.Build.DenseOfArray(A_array).Transpose();

                var B_array = Extension.Sub(detector_center, CrossR);
                var B = Vector<double>.Build.DenseOfArray(B_array).ToColumnMatrix();
                A = A.Inverse();
                var cross_t = A.Multiply(B).ToRowMajorArray();
                
                var cross_point = detector_cross_line.Select(x => x * cross_t[1]).ToArray();
                limit_line_cross_point.Add(cross_point);

                var cross_position = Extension.vectorLenght(cross_point);
                position.Add (mult * cross_position);
                limit_line_position.Add(mult * cross_position);
            }
            var limit_wave_length = new double[] { waves.Max(), waves.Min() };

            waveLimits = new WaveLimits();
            waveLimits.max = new WaveLimit()
            {
                lambda = Math.Round(waves.Max(), 5),
                position= Math.Round(position[1], 5),
            };

            waveLimits.min = new WaveLimit()
            {
                lambda = Math.Round(waves.Min(),5),
                position = Math.Round(position[0],5)
            };

            onWaveLimitChange?.Invoke(this, waveLimits);
        }

        private void GetRotationAngle(double phi)
        {
            double[] Cp = { 0.0, -crystalR / 2.0 };
            double[] Sp = { -SrcDist * Math.Sin(phi), -SrcDist * Math.Cos(phi) };
            double[] Dp = { DstDist * Math.Sin(phi), -DstDist * Math.Cos(phi) };

            double[] Dn = { Dp[0] - Sp[0], Dp[1] - Sp[1] };
            Dn = new double[]{ -Dn[1], Dn[0]};
            double[] CDp = { Cp[0] - Dp[0], Cp[1] - Dp[1] };

            var Dnl = Extension.vectorLenght(Dn);
            var CDpl = Extension.vectorLenght(CDp);

            Dn = Dn.Select(d => d / Dnl).ToArray();
            CDp = CDp.Select(d => d / CDpl).ToArray();

            var RotAngleCos = (Dn[0] * CDp[0] + Dn[1] * CDp[1]);

            FilmRotationAngle = Math.Acos(RotAngleCos) * 180.0 / Math.PI;
        }

        private Tuple<double, double, int> GetCrystalInfo(string path)
        {
            var io = new StreamReader(path);
            List<Tuple<double, double>> points = new List<Tuple<double, double>>();

            int format_shift = 0;

            while (!io.EndOfStream)
            {
                var rline = io.ReadLine().Trim();
                rline = System.Text.RegularExpressions.Regex.Replace(rline, @"\s+", " ");
                if (rline.Contains("#v2"))
                {
                    format_shift += 1;
                    continue;
                }

                var line = rline.Split(" \t".ToArray());
                try
                {
                    points.Add(new Tuple<double, double>(double.Parse(line[format_shift + 0]), double.Parse(line[format_shift + 1])));
                }
                catch(Exception e)
                {
                    Logger.Warning(e.ToString());
                    return null;
                }
            }

            var size = points.Count;
            var max = points.Max(x => x.Item1);
            var min = Math.Abs(points.Min(x => x.Item1));
            var wh = (max + min) / 2.0;

            var step = Math.Abs(points[0].Item1 - points[1].Item1);

            io.Close();
            return new Tuple<double, double, int>(wh, step, size);
        }
    }
}
