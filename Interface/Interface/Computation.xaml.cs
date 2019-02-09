using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Interface
{
    /// <summary>
    /// Логика взаимодействия для Computation.xaml
    /// </summary>
    public partial class Computation : Window
    {
        private SystemConfig m_self;
        private Wave[] m_wave;
        public bool isOk;

        public event EventHandler<Wave> WaveProcessed;

#region cfunction
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        private struct RaysStepInfo
        {
            public int linkedLibraryMinorOutput;
            public int linkedLibraryTotalOutput;
            public int totalWaves;
            public int currentWaves;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        private struct WaveTraceResult
        {
            public double lambda;
            public double dlambda;
            public double intensity;
            public double generate;
            public double captured;
            public double reflected;
            public double relfectivity;
        }

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private delegate void RayProgressCallback(RaysStepInfo value);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private delegate void WaveTracedCallback(WaveTraceResult value);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private delegate void StdOutCallback(IntPtr ptr, UInt64 count);

        [DllImport(@"./sys/xray-tracing.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int RayTracing(int argc, [MarshalAs(UnmanagedType.LPStr)] string argv, 
            [MarshalAs(UnmanagedType.FunctionPtr)] RayProgressCallback callbackPointer,
            [MarshalAs(UnmanagedType.FunctionPtr)] WaveTracedCallback waveCallbackPointer,
            [MarshalAs(UnmanagedType.FunctionPtr)] StdOutCallback stdOutCallback);

        [DllImport(@"./sys/xray-tracing.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void xrt_terminate();
        #endregion

        private String parFileName = System.IO.Directory.GetCurrentDirectory() + "/par/Order_0.par";

        private class OutStreamWriter : System.IO.TextWriter
        {
            public override Encoding Encoding { get { return Encoding.UTF8; } }

            public event EventHandler<string> OnConsoleOut;
            public override void Write(string str)
            {
                OnConsoleOut?.Invoke(this, str);
                base.Write(str);
            }
            public override void WriteLine(string str)
            {
                OnConsoleOut?.Invoke(this, str + "\n");
                base.WriteLine(str);
            }
            public override void Write(char str)
            {
                OnConsoleOut?.Invoke(this, ""+str);
                base.Write(str);
            }
        }
        public Computation(ref SystemConfig sysConf, Wave[] waveSet)
        {
            InitializeComponent();
            m_self = sysConf;
            m_wave = waveSet;

            isOk = PrepareComputation();
        }

        private bool PrepareComputation()
        {
            var fileName = parFileName;
            var outFile = new System.IO.StreamWriter(fileName);

            var idx = m_self.mainOrder - 1;
            try
            {
                outFile.Write("$GenName = " + "Order_0\n");
                outFile.Write("$rayCnt = " + Extension.mstr(m_self.RayCount) + "\n");
                outFile.Write("$rayByIt = " + Extension.mstr(m_self.RayByIter) + "\n");
                outFile.Write("$cR = " + Extension.mstr(m_self.crystalR) + "\n");
                outFile.Write("$wA = " + Extension.mstr(m_self.crystalW) + "\n");
                outFile.Write("$hA = " + Extension.mstr(m_self.crystalH) + "\n");
                outFile.Write("$ScattAngle = " + Extension.mstr(m_self.BraggA) + "\n");
                outFile.Write("$Cr2D = " + Extension.mstr(m_self.CrystalProps.crystal2d[idx] / m_self.mainOrder) + "\n");
                outFile.Write("$DistrWidth = " + Extension.mstr(m_self.CrystalProps.DistrHW[idx]) + "\n");
                outFile.Write("$RFile = " + m_self.CrystalProps.DistrFileName[idx] + "\n");
                outFile.Write("$RSize = " + Extension.mstr(m_self.CrystalProps.DistrSize[idx]) + "\n");
                outFile.Write("$RStep = " + Extension.mstr(m_self.CrystalProps.DistrStep[idx]) + "\n");
                outFile.Write("$SrcDist = " + Extension.mstr(m_self.SrcDist) + "\n");
                outFile.Write("$SrcCone = " + Extension.mstr(m_self.SrcCone) + "\n");
                outFile.Write("$DDist  = " + Extension.mstr(m_self.DstDist) + "\n");
                outFile.Write("$FilmA  = " + Extension.mstr(m_self.FilmAngle) + "\n");
                outFile.Write("$FildDir = " + Extension.mstr(m_self.ToFilmDirection) + "\n");

                outFile.Write("$SrcSizeW = " + Extension.mstr(m_self.SrcSizeW / 2.0) + "\n");
                outFile.Write("$SrcSizeH = " + Extension.mstr(m_self.SrcSizeH / 2.0) + "\n");
                outFile.Write("$FilmH = " + Extension.mstr(m_self.FilmSizeH) + "\n");
                outFile.Write("$FilmW = " + Extension.mstr(m_self.FilmSizeW) + "\n");

                foreach (var object_props in m_self.Object.GetType().GetProperties())
                {
                    outFile.Write(String.Format("${0} = {1}\n", object_props.Name, Extension.mstr(object_props.GetValue(this.m_self.Object))));
                }

                outFile.Write("$MrType = " + m_self.CrystType + "\n");

                outFile.Write("$WLCount  = " + Extension.mstr(m_wave.Length) + "\n");

                outFile.Write("\n\n\n");

                var pattern = new System.IO.StreamReader(@"sys/pattern.par");
                var lines = pattern.ReadToEnd();
                pattern.Close();

                outFile.Write(lines);
                outFile.Write("\n\n\n");


                var waveIndex = 1;
                foreach (var wave in m_wave)
                {
                    outFile.Write("wave" + Extension.mstr(waveIndex) + " = " + Extension.mstr(wave.lambda) + "\n");
                    outFile.Write("dwave" + Extension.mstr(waveIndex) + " = " + Extension.mstr(wave.dlambda * 1e-3) + "\n");
                    outFile.Write("iwave" + Extension.mstr(waveIndex) + " = " + Extension.mstr(wave.intensity) + "\n");
                    waveIndex += 1;
                }
                return true;
            }
            catch (Exception e)
            {
                Logger.Error(e.ToString());
                return false;
            }
            finally
            {
                outFile.Close();
            }
        }

        public async Task<bool> StartXRayTracing()
        {
            RayProgressCallback rpc = (value) =>
            {
                Dispatcher.Invoke(new Action(() =>
                {
                    WaveProgress.Maximum = value.totalWaves;
                    WaveProgress.Value = value.currentWaves;
                    WaveProgressLabel.Content = value.currentWaves.ToString() + "/" + value.totalWaves.ToString();

                    RaysProgres.Maximum = value.linkedLibraryTotalOutput;
                    RaysProgres.Value = value.linkedLibraryMinorOutput;
                    RaysProgresLabel.Content = value.linkedLibraryMinorOutput.ToString() + "/" + value.linkedLibraryTotalOutput.ToString();
                }));
            };
            /*try
            {
                System.IO.Directory.Delete("results/Order_0", true);
            }
            catch(Exception expc) { }*/
            //var config_file = System.IO.Directory.GetCurrentDirectory() + parFileName;
            await Task<int>.Factory.StartNew(() =>   RayTracing(1, parFileName, rpc, WaveTraced, stdOutCallback));

            return false;
        }

        private void stdOutCallback(IntPtr ptr, UInt64 count)
        {
            string Message = Marshal.PtrToStringAnsi(ptr, (int)count);
            Dispatcher.Invoke(new Action(() => {
                XRayTraceOutput.AppendText(Message+"\n");
                XRayTraceOutput.ScrollToEnd();
                }));
        }

        private void P_OutputDataReceived(object sender, System.Diagnostics.DataReceivedEventArgs e)
        {
            Dispatcher.Invoke(new Action(() => XRayTraceOutput.AppendText(e.Data.ToString())));
        }

        private void WaveTraced(WaveTraceResult waveInfo)
        {
            DataBaseWavelength dbWave = new DataBaseWavelength()
            {
                lambda = waveInfo.lambda,
                dlambda = waveInfo.dlambda,
                intensity = waveInfo.intensity
            };
            Wave tracedWave = new Wave(dbWave)
            {
                Emited = waveInfo.generate,
                Reflected = waveInfo.reflected,
                Efficiency = waveInfo.reflected / waveInfo.generate * (m_self.SolidCone / (4.0 * Math.PI)),
            };
            WaveProcessed?.Invoke(this, tracedWave);
        }

        private void Expander_Expanded(object sender, RoutedEventArgs e)
        {
            var expander = sender as Expander;
            this.Height += expander.Height * 0.8;
        }

        private void Expander_Collapsed(object sender, RoutedEventArgs e)
        {
            var expander = sender as Expander;
            this.Height -= expander.Height * 0.8;
        }

        private void Terminate_Click(object sender, RoutedEventArgs e)
        {
            xrt_terminate();
        }
    }
}
