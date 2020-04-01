using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections;
using Newtonsoft.Json;
using Microsoft.Win32;
using OxyPlot.Wpf;

namespace Interface
{
    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
    /// 
    public class UIList
    {
        public DataGrid dgrid;
        public WavePlotter wavePlot;
        public DetectorPlot detectorPlot;
        public Detector detector;
    }
    public partial class MainWindow : Window
    {
        private WaveLengthLoader m_WavesDataBase;
        private WaveLenghts m_SelectedWaves;
        private WavePlotter m_WavePlot;
        private SystemConfig m_SysConf;
        private ConfigPlotter m_ConfPlot;
        private CrystalPlot m_CrystalPlot;
        private Detector m_Detector;
        private DetectorPlot m_DetectorPlot;

        private UIList m_UIS;


        public MainWindow()
        {
            InitializeComponent();

            System.Globalization.CultureInfo.DefaultThreadCurrentCulture = new System.Globalization.CultureInfo("en-US");
            System.Globalization.CultureInfo.DefaultThreadCurrentUICulture = new System.Globalization.CultureInfo("en-US");

            m_SelectedWaves = new WaveLenghts();
            m_SelectedWaves.onWaveAdd += SelectedWaves_onWaveAdd;
            m_SelectedWaves.onWaveChange += SelectedWaves_onWaveChange;

            m_SysConf = new SystemConfig();
            m_SysConf.onChange += SysConf_onChange;
            m_SysConf.onWaveLimitChange += SysConf_onWaveLimitChange;

            this.DataContext = m_SysConf;

            m_WavePlot = new WavePlotter(WavePlotCanvas);
            m_ConfPlot = new ConfigPlotter(ConfigurationPlot);

            m_CrystalPlot = new Interface.CrystalPlot(CrystalPlotViev);

            m_Detector = new Detector();
            m_DetectorPlot = new DetectorPlot(DetectorPlotCanvas, ObservedSpectrumCanvas);
            m_DetectorPlot.XZoom += DetectorPlot_XZoom;
            m_DetectorPlot.YZoom += DetectorPlot_YZoom;

            m_UIS = new UIList()
            {
                dgrid = SelectedWavesList,
                wavePlot = m_WavePlot,
                detector = m_Detector,
                detectorPlot = m_DetectorPlot
            };

            Logger.InitLogger("ExceptionLog.txt");
        }

        private void DetectorPlot_YZoom(object sender, DetectorPlot.Point e)
        {
            m_Detector.MaxDetectorPositionSag = e.max;
            m_Detector.MinDetectorPositionSag = e.min;
            UpdateDatectorLimsContent();
        }

        private void DetectorPlot_XZoom(object sender, DetectorPlot.Point e)
        {
            m_Detector.MaxDetectorPositionMer = e.max;
            m_Detector.MinDetectorPositionMer = e.min;
            UpdateDatectorLimsContent();
        }

        private void SelectedWaves_onWaveChange(object sender, EventArgs e)
        {
            Dispatcher.Invoke(new Action(() => SelectedWaves_onWaveAdd(sender, e)));
        }

        private void SysConf_onWaveLimitChange(object sender, SystemConfig.WaveLimits e)
        {
            m_WavePlot.SetWaveLimits(e);
        }

        private void SysConf_onChange(object sender, EventArgs e)
        {
            this.DataContext = "";
            this.DataContext = m_SysConf;
            UpdateConfigurationOutput();
            m_ConfPlot.DrawConfiguration(m_SysConf);
        }

        private void SelectedWaves_onWaveAdd(object sender, EventArgs e)
        {
            m_SelectedWaves.UpdateUI(m_UIS);
        }

        private void WaveSearch_TextChanged(object sender, TextChangedEventArgs e)
        {
            var searchPattern = WaveSearch.Text;
            m_WavesDataBase.Search(searchPattern, WavesDatabaseList);
        }

        private void WaveDataBaseSelection_FileSelect(object sender, OpenFileButton.FileSelectionArgs e)
        {
            if (e.isSelected)
            {
                m_WavesDataBase = new WaveLengthLoader(e.FileName);
                m_WavesDataBase.Search(" ", WavesDatabaseList);
            }
        }

        private void CrystalConfigFile_FileSelect(object sender, OpenFileButton.FileSelectionArgs e)
        {
            if (e.isSelected)
            {
                m_SysConf.ReadCrystalInfo(e.FileName);
            }
        }

        private void WavesDatabaseList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            var table = sender as DataGrid;
            if (table.SelectedItem != null)
                m_SelectedWaves.Add(table.SelectedItem as DataBaseWavelength, 1);
        }

        private void WaveDataBaseSelection_Loaded(object sender, RoutedEventArgs e)
        {
            if ((sender as OpenFileButton).SelectedFileName != null)
            {
                m_WavesDataBase = new WaveLengthLoader((sender as OpenFileButton).SelectedFileName);
                m_WavesDataBase.Search(" ", WavesDatabaseList);
            }
        }

        private void ConfValue_SourceUpdated(object sender, DataTransferEventArgs e)
        {
            try
            {
                if ((sender as TextBox) == CentralWaveLenght)
                    m_SysConf.UpdateConfigurationWithNewCentralWave();
                if ((sender as TextBox) == Dst2Src)
                    m_SysConf.UpdateConfigurationWithNewDetectorDistance();
                else
                    m_SysConf.UpdateConfiguration();

                if (m_Detector.isReady)
                {
                    var scalef = m_SysConf.CrystalProps.crystal2d[0] * m_SysConf.mainOrder / (1.0 * m_SysConf.CrystalProps.crystal2d[m_SysConf.mainOrder - 1]);
                    m_DetectorPlot.PlotSpectrum(m_Detector, scalef);
                }
            }
            catch (Exception exc)
            {
                ConfigurationOuput.Text = exc.Message + "\n" + exc.StackTrace;
            }
            catch{
                ConfigurationOuput.Text = "Catch undefined exception.";
            }
        }

        private void FSSR1_Scheme_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                m_SysConf.ConstructFSSR1();
            }
            catch (Exception exc)
            {
                ConfigurationOuput.Text = exc.Message + "\n" + exc.StackTrace;
            }
        }

        private void UpdateConfig_Click(object sender, RoutedEventArgs e)
        {
            ConfValue_SourceUpdated(sender, null);
        }

        private void AddNewLine_Click(object sender, RoutedEventArgs e)
        {
            WavesDatabaseList_MouseDoubleClick(WavesDatabaseList, null);
        }

        private void DeleteLine_Click(object sender, RoutedEventArgs e)
        {
            var table = SelectedWavesList;
            if (table.SelectedItem != null)
            {
                this.m_SelectedWaves.Del(table.SelectedItem as Wave);
            }
        }

        private void DeleteAll_Click(object sender, RoutedEventArgs e)
        {
            m_SelectedWaves.Clear();
        }

        private void SelectedWavesList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Right)
            {
                m_SelectedWaves.Add(new DataBaseWavelength()
                {
                    name = "Added Wave",
                    lambda = 0.0,
                    dlambda = 0.0,
                    intensity = 1.0,
                }, 1);
            }
        }

        private void RadioButton_Click(object sender, RoutedEventArgs e)
        {
            var wave = m_SelectedWaves.GetZeroWave();//SelectedWavesList.Se as Wave;
            if (m_SysConf != null)
            {
                m_SysConf.ZeroWaveStr = wave.name + " " + wave.lambda.ToString("0.#####");
                m_SysConf.zeroWave = wave.lambda;
                m_SysConf.centralWave = wave.lambda;
            }

            ConfValue_SourceUpdated(CentralWaveLenght, null);

            if (m_Detector.isReady)
            {
                SetLinesFeatures(m_SelectedWaves.Wave.ToArray());
                var scalef = m_SysConf.CrystalProps.crystal2d[0] * m_SysConf.mainOrder / (m_SysConf.mainOrder * m_SysConf.CrystalProps.crystal2d[m_SysConf.mainOrder - 1]);
                m_DetectorPlot.PlotSpectrum(m_Detector, scalef);
            }
        }

        private void GenerateAdditionalLine_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var orderStr = AdditionalOrdersList.Text.Split(' ');
                var orders = orderStr.Select(x => { int value = 0; int.TryParse(x, out value); return value; }).ToList();

                orders.Add(m_SysConf.mainOrder);
                m_SelectedWaves.GenerateAdditionalWaves(orders, m_SysConf);
            }
            catch (Exception exc)
            {
                Logger.Error(exc.ToString());
                return;
            }
        }

        private void SaveConfig_Click(object sender, RoutedEventArgs e)
        {
            string jsonConf = Newtonsoft.Json.JsonConvert.SerializeObject(m_SysConf);

            var debugPath = "../../json.json";

            string configFilePath = debugPath;

            var saveDialog = new SaveFileDialog();
            saveDialog.Filter = "Config files (*.xconf)|*.xconf|All file (*.*)|*.*";

            if (saveDialog.ShowDialog() == false)
                return;

            configFilePath = saveDialog.FileName;

            System.IO.StreamWriter sw = new System.IO.StreamWriter(configFilePath);
            sw.WriteLine(jsonConf);

            jsonConf = Newtonsoft.Json.JsonConvert.SerializeObject(m_WavesDataBase);
            sw.WriteLine(jsonConf);

            jsonConf = Newtonsoft.Json.JsonConvert.SerializeObject(m_SelectedWaves);
            sw.WriteLine(jsonConf);

            sw.Close();
        }

        private void LoadConfig_Click(object sender, RoutedEventArgs e)
        {
            var debugPath = "../../json.json";

            string configFilePath = debugPath;

            var openDialog = new OpenFileDialog();
            openDialog.Filter = "Config files (*.xconf)|*.xconf|All file (*.*)|*.*";

            if (openDialog.ShowDialog() == false)
                return;

            configFilePath = openDialog.FileName;

            System.IO.StreamReader sw = new System.IO.StreamReader(configFilePath);
            var jsonConf = sw.ReadLine();
            m_SysConf = JsonConvert.DeserializeObject<SystemConfig>(jsonConf);

            jsonConf = sw.ReadLine();
            var waveBase = JsonConvert.DeserializeObject<WaveLengthLoader>(jsonConf);
            m_WavesDataBase = new WaveLengthLoader(waveBase);

            jsonConf = sw.ReadLine();
            m_SelectedWaves = JsonConvert.DeserializeObject<WaveLenghts>(jsonConf);

            m_SelectedWaves.onWaveAdd += SelectedWaves_onWaveAdd;
            m_SelectedWaves.onWaveChange += SelectedWaves_onWaveChange;

            m_SysConf.onChange += SysConf_onChange;
            m_SysConf.onWaveLimitChange += SysConf_onWaveLimitChange;

            SysConf_onChange(this, null);
            m_SelectedWaves.UpdateUI(m_UIS);
            m_WavePlot.SetWaveLimits(m_SysConf.waveLimits);
        }

        private async void Trace_Click(object sender, RoutedEventArgs e)
        {
            if (m_SysConf.waveLimits == null)
                return;

            var PreparedWaves = m_SelectedWaves.GetWaveInRagne(m_SysConf.waveLimits);

            if (PreparedWaves.Length == 0)
                return;

            Computation c = new Computation(ref m_SysConf, PreparedWaves);
            c.Title = "XRay-tracing";
            c.WaveProcessed += OnWaveProcessed;

            if (c.isOk == false)
                return;

            this.IsEnabled = false;

            c.Show();

            CalculationProgressBar.Value = 0;
            CalculationProgressLabel.Content = "Tracing...";

            try
            {
                bool res = await c.StartXRayTracing();
            }
            catch (Exception exc)
            {
                Logger.Error("C Trace Exception. " + exc.Message + "\n" + exc.StackTrace);
            }

            c.Close();

            PerformAnalis(PreparedWaves);

            this.IsEnabled = true;
        }

        private async void PerformAnalis(Wave[] WaveList)
        {
            CalculationProgressLabel.Content = "Read crystal plane.";
            CalculationProgressBar.Value = 2;

            string crystalFileName = "results/Order_0.dump";
            await Task.Factory.StartNew(() => m_CrystalPlot.ReadCrystalPlane(crystalFileName));
            m_CrystalPlot.PlotMirror(m_SysConf.crystalW, m_SysConf.crystalH);

            CalculationProgressLabel.Content = "Read detector plane.";
            CalculationProgressBar.Value = 3;
            string detectorFile = "results/Order_0/Detector.dmp";
            await Task.Factory.StartNew(() => m_Detector.ReadDetectorPlane(detectorFile));

            CalculationProgressLabel.Content = "Build spectrum.";
            CalculationProgressBar.Value = 4;
            await Task.Factory.StartNew(() => m_Detector.BuildHistogram());

            CalculationProgressLabel.Content = "Build detector heatmap.";
            CalculationProgressBar.Value = 5;
            await Task.Factory.StartNew(() => m_Detector.BuildHeatMap());

            CalculationProgressLabel.Content = "Calculate line position, magnification, fwhm.";
            CalculationProgressBar.Value = 6;
            await Task.Factory.StartNew(() => SetLinesFeatures(WaveList));
           
            CalculationProgressLabel.Content = "Provide approximation of Magnification and Efficiency.";
            CalculationProgressBar.Value = 6;
            await ProvideAdditionalApproximation(WaveList);

            LogConfiguration();

            UpdateDatectorLimsContent();

            PixelSize.DataContext = "";
            PixelSize.DataContext = m_Detector;

            m_DetectorPlot.PlotDetector(m_Detector);
            var scalef = m_SysConf.CrystalProps.crystal2d[0] * m_SysConf.mainOrder / (m_SysConf.mainOrder * m_SysConf.CrystalProps.crystal2d[m_SysConf.mainOrder - 1]);
            m_DetectorPlot.PlotSpectrum(m_Detector, scalef);

            CalculationProgressLabel.Content = "Done.";
            CalculationProgressBar.Value = 8;

            //ConfigurationOuput.Text += LogConfiguration();
            UpdateConfigurationOutput();
        }

        private void UpdateConfigurationOutput()
        {
            ConfigurationOuput.Text = m_SysConf.GenerateOutputConfig();
            ConfigurationOuput.Text += LogConfiguration();
        }

        public string LogConfiguration()
        {
            var data = "\n\n[RESULTS]\n";
            data += "Reflection limits for " + m_SysConf.mainOrder + " order\n";
            data += "Min wavelength = " + m_SysConf.waveLimits?.min.lambda.ToString() + "\tPosition = " + m_SysConf.waveLimits?.min.position.ToString() + "\n";
            data += "Max wavelength = " + m_SysConf.waveLimits?.max.lambda.ToString() + "\tPosition = " + m_SysConf.waveLimits?.max.position.ToString() + "\n";
            data += "Reference line = " + m_SysConf.ZeroWaveStr + "\n"; ;
            data += "Dispersion curve: " + m_SelectedWaves.GetZeroWave()?.Curve?.ToString() + "\n";
            
            data += "***************************************************************\n";
            data += "Name\tWavelength\tEmitted\tReflected\tEfficiency\tXCoords\tMagnification\tFWHM\tOrder\tLine Width\n";
            foreach (var w in m_SelectedWaves.Wave)
            {
                data += String.Format("{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}\t{7}\t{8}\t{9}\n",
                    w.name, Extension.mstr(w.lambda), Extension.mstr(w.Emited),
                    Extension.mstr(w.Reflected), w.Efficiency,
                    Extension.mstr(w.XCoord), Extension.mstr(w.Magnification),
                    Extension.mstr(w.FWHM), Extension.mstr(w.Order),
                    Extension.mstr(w.FWHM));
            }

            data += "\n\nDispercion curves for the following reference lines\n";
            foreach (var l in m_SelectedWaves.Wave) {
                data += "For L_ref = " + Extension.mstr(l.lambda) + ":\t" + l.Curve?.ToString() + "\n";
            }

            if (m_AdditionalApproximation != null)
            {
                data += "\n\nAdditional fits\n";
                data += "Efficiency curve:" + m_AdditionalApproximation?.m_EfficiencyFit.ToString() + "\n";
                data += "Magnification curve:" + m_AdditionalApproximation?.m_MagnificationFit.ToString() + "\n";
                if (m_AproximatedWavePoints != null)
                {
                    data += "***************************************************************\n";
                    data += "Name\tWavelength\tEfficiency\tFit_Efficiency\tEfficiency_fit_quality\tMagnification\tFit_Magnification\tMagnification_fit_quality\n";
                    foreach (var w in m_AproximatedWavePoints)
                    {
                        data += String.Format("{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}\t{7}\n",
                            w.name, Extension.mstr(w.lambda),
                            w.Efficiency, w.Approx_Efficiency, Extension.mstr(Math.Abs(w.Approx_Efficiency - w.Efficiency) / w.Efficiency),
                            Extension.mstr(w.Magnification), Extension.mstr(w.Approx_Magnification), Extension.mstr(Math.Abs(w.Approx_Magnification - w.Magnification) / w.Magnification)
                            );
                    }
                }
            }

            data += m_SysConf.GetCrystalConfig();

            return data;
        }

        private void SetLinesFeatures(Wave[] WaveList)
        {
            var ZeroWaveFeatures = m_Detector.GetLineFeatures(m_SelectedWaves.GetZeroWave());
            if (ZeroWaveFeatures == null)
                return;

            foreach (var wave in WaveList)
            {
                var feature = m_Detector.GetLineFeatures(wave);

                if (feature == null)
                    continue;

                feature.XCoord -= ZeroWaveFeatures.XCoord;
                feature.Size /= m_SysConf.SrcSizeH;

                m_SelectedWaves.AddFeatures(feature, wave);

                wave.FWHM = feature.FWHM;
                wave.XCoord = feature.XCoord;
                wave.Magnification = feature.Size;
                wave.Curve = feature.Curve;
            }

            m_Detector.SetZeroWave(m_SelectedWaves.GetZeroWave());
        }

        private void UpdateDatectorLimsContent()
        {
            YMin.DataContext = "";
            YMin.DataContext = m_Detector;

            XMin.DataContext = "";
            XMin.DataContext = m_Detector;

            YMax.DataContext = "";
            YMax.DataContext = m_Detector;

            XMax.DataContext = "";
            XMax.DataContext = m_Detector;
        }

        private void OnWaveProcessed(object sender, Wave e)
        {
            m_SelectedWaves.UpdateWave(e);
        }

        private void DetectorChange_SourceUpdated(object sender, DataTransferEventArgs e)
        {
            m_Detector.BuildHeatMap();
            m_DetectorPlot.PlotDetector(m_Detector);
            m_DetectorPlot.PlotSpectrum(m_Detector);
        }

        private void CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            if (m_WavePlot != null)
            {
                m_WavePlot.FullRange = (bool)(sender as CheckBox).IsChecked;
                m_SelectedWaves.UpdateUI(m_UIS);
            }
        }

        private void RadioButton_Checked(object sender, RoutedEventArgs e)
        {
            var wave = (sender as RadioButton).DataContext as Wave;
            if (wave != null)
                wave.isZeroWave = true;
        }

        private void RadioButton_Unchecked(object sender, RoutedEventArgs e)
        {
            var wave = (sender as RadioButton).DataContext as Wave;
            if (wave != null)
                wave.isZeroWave = false;
        }

        private void DeleteAdditionalLine_Click(object sender, RoutedEventArgs e)
        {
            m_SelectedWaves.RemoveAdditionalWaves();
        }

        private void SaveResuts_Click(object sender, RoutedEventArgs e)
        {
            var saveDialog = new SaveFileDialog();
            saveDialog.Filter = "Text file (*.txt)|*.txt|All file (*.*)|*.*";

            if (saveDialog.ShowDialog() == false)
                return;

            var FileName = saveDialog.FileName;
            System.IO.StreamWriter sw = new System.IO.StreamWriter(FileName);
            sw.Write(ConfigurationOuput.Text.Replace("\n","\r\n"));
            sw.Close();
        }

        private void SaveDetectorPlane(string path)
        {
            
                var xrange = new Extension.Range()
                {
                    VMax = m_Detector.MaxDetectorPositionMer,
                    VMin = m_Detector.MinDetectorPositionMer
                };

                var yrange = new Extension.Range()
                {
                    VMax = m_Detector.MaxDetectorPositionSag,
                    VMin = m_Detector.MinDetectorPositionSag
                };

                var heatMap = Extension.Histogram2D(m_Detector.Points.Select(p => p.x), m_Detector.Points.Select(p => p.y), xrange, yrange, m_Detector.PixelSize);

                var w = heatMap.GetLength(0);
                var h = heatMap.GetLength(1) - 2;

                var unbounded = RemoveBound(heatMap);

                Extension.SaveTiff(path, unbounded, w, h);
        }

        private void SaveCrystalPlane(string path)
        {
            var xrange = new Extension.Range()
            {
                VMax = m_SysConf.crystalW / 2,
                VMin = -m_SysConf.crystalW / 2
            };

            var yrange = new Extension.Range()
            {
                VMax = m_SysConf.crystalH / 2,
                VMin = -m_SysConf.crystalH / 2
            };

            var heatMap = Extension.Histogram2D(m_CrystalPlot.Points.Select(p => p.X), m_CrystalPlot.Points.Select(p => p.Y), xrange, yrange, m_SysConf.crystalW / 1000);

            var w = heatMap.GetLength(0);
            var h = heatMap.GetLength(1) - 2;

            var unbounded = RemoveBound(heatMap);

            Extension.SaveTiff(path, unbounded, w, h);
        }

        private void SaveImages_Click(object sender, RoutedEventArgs e)
        {
            var saveDialog = new SaveFileDialog();
            saveDialog.Filter = "TIFF file (*.tiff)|*.tiff";

            if (saveDialog.ShowDialog() == false)
                return;

            var path = saveDialog.FileName;

            //
            try
            {
                var detectorImagePath = System.IO.Path.GetDirectoryName(path) + "\\" + System.IO.Path.GetFileNameWithoutExtension(path) + "_detector" + System.IO.Path.GetExtension(path);
                SaveDetectorPlane(detectorImagePath);
            }
            catch (Exception exc) {
                Logger.Error(exc.ToString());
            }
            //

            //
            try
            {
                var crystalImagePath = System.IO.Path.GetDirectoryName(path) + "\\" +  System.IO.Path.GetFileNameWithoutExtension(path) + "_crystal" + System.IO.Path.GetExtension(path);
                SaveCrystalPlane(crystalImagePath);
            }
            catch (Exception exc) {
                Logger.Error(exc.ToString());
            }
            //

            /*var spectrumImagePath = System.IO.Path.GetDirectoryName(path) + "\\" +  System.IO.Path.GetFileNameWithoutExtension(path) + "_spectrum" + ".png";
            var pngExporter = new OxyPlot.Wpf.PngExporter { Width = 600, Height = 400, Background = OxyPlot.OxyColors.White };
            pngExporter.ExportToFile(ObservedSpectrumCanvas.ActualModel, spectrumImagePath);*/
        }

        private Int16[] RemoveBound(double[,] heatMap)
        {
            List<Int16> detector = new List<Int16>();
            for (int i = 0; i < heatMap.GetLength(0); ++i)
                for (int j = 1; j < heatMap.GetLength(1) - 1; ++j)
                    detector.Add((Int16)heatMap[i, j]);
            return detector.ToArray();
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            Logger.StopLogger();
        }

        private void ObjectMap_FileSelect(object sender, OpenFileButton.FileSelectionArgs e)
        {
            if (e.isSelected)
            {
                this.m_SysConf.Object.GridMap = e.FileName;
                this.m_SysConf.ObjectExist = true;
                m_SysConf.Object.GridType = "manual";
                SysConf_onChange(this, null);
            }
        }

        private void OnRemoveObject_Click(object sender, RoutedEventArgs e)
        {
            ObjectFileSelection.clear();
            this.m_SysConf.ObjectExist = false;
            m_SysConf.Object = new SystemConfig.ObjectInfo();

            SysConf_onChange(this, null);
        }

        private void ConfigurationPlot_Loaded(object sender, RoutedEventArgs e)
        {
            m_ConfPlot.DrawConfiguration(m_SysConf);
        }

        private void HelpAbout_Click(object sender, RoutedEventArgs e)
        {
            string build_date = "Apr 1 2020";
            String info = "WPF XRay-tracing build info\n" +
                          "Ray-tracing core: " + Computation.CoreBuildDate() + "\n" +
                          "WPF Interface: " + build_date;
            MessageBox.Show(info, "About", MessageBoxButton.OK);
        }

        private void DataGridContextCreateline_Click(object sender, RoutedEventArgs e)
        {
            m_SelectedWaves.Add(new DataBaseWavelength()
            {
                name = "New wave",
                lambda = 0.0,
                dlambda = 0.0,
                intensity = 1.0,
            }, 1);
        }
    }

}
