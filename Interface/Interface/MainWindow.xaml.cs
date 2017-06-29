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
    }
    public partial class MainWindow : Window
    {
        private WaveLengthLoader m_WavesDataBase;
        private WaveLenghts m_SelectedWaves;
        private WavePlotter m_WavePlot;
        private SystemConfig m_SysConf;
        private ConfigPlotter m_ConfPlot;
        private CrystalPlot m_CrystalPlot;

        private UIList m_UIS;
        public MainWindow()
        {
            InitializeComponent();
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

            m_UIS = new UIList()
            {
                dgrid = SelectedWavesList,
                wavePlot = m_WavePlot
            };
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
            ConfigurationOuput.Text = m_SysConf.GenerateOutputConfig();
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
                else
                    m_SysConf.UpdateConfiguration();
            }
            catch (Exception exc)
            {
                ConfigurationOuput.Text = exc.Message + "\n" + exc.StackTrace;
            }
        }

        private void FSSR1_Scheme_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                m_SysConf.ConstructFSSR1();
            }
            catch(Exception exc)
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
            var wave = SelectedWavesList.SelectedItem as Wave;
            m_SysConf.ZeroWaveStr = wave.name + " " + wave.lambda.ToString("0.#####");
            m_SysConf.zeroWave = wave.lambda;
            m_SysConf.centralWave = wave.lambda;

            ConfValue_SourceUpdated(CentralWaveLenght, null);
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

        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            if (m_SysConf.waveLimits == null)
                return;

            Computation c = new Computation(ref m_SysConf, m_SelectedWaves.GetWaveInRagne(m_SysConf.waveLimits));
            c.Title = "XRay-tracing";
            c.WaveProcessed += OnWaveProcessed;

            if (c.isOk == false)
                return;

            this.IsEnabled = false;

            c.Show();

            bool res = await c.StartXRayTracing();

            c.Close();

            PerformAnalis();

            this.IsEnabled = true;
        }

        private async void PerformAnalis()
        {
            string crystalFileName = "results/Order_0.dump";
            await Task.Factory.StartNew( () => m_CrystalPlot.ReadCrystalPlane(crystalFileName));

            m_CrystalPlot.PlotMirror(m_SysConf.crystalW, m_SysConf.crystalH);
        }

        private void OnWaveProcessed(object sender, Wave e)
        {
            m_SelectedWaves.UpdateWave(e);
        }
    }

    public static class Extension
    {
        public static void Search(this WaveLengthLoader loader, string pattern, DataGrid WavesDatabaseList)
        {
            WavesDatabaseList.ItemsSource = loader.Search(pattern);
            WavesDatabaseList.Items.Refresh();
        }

        public static void UpdateUI(this WaveLenghts waves, UIList ui)
        {
            ui.dgrid.ItemsSource = waves.Wave;
            ui.dgrid.Items.Refresh();

            ui.wavePlot.PlotLines(waves.Wave);
        }

        public static string mstr(object value)
        {
            return value?.ToString();
        }

        public static string mstr(double value)
        {
            return (value).ToString("0.#####");
        }

        public static double vectorLenght(double[] v)
        {
            return Math.Sqrt(v[0] * v[0] + v[1] * v[1]);
        }

        public static double[] Sub(double[] a, double[] b)
        {
            double[] c = { a[0] - b[0], a[1] - b[1]};
            return c;
        }

        public static double[] Mul(double[] a, double[] b)
        {
            double[] c = { a[0] * b[0], a[1] * b[1] };
            return c;
        }

        public static double dot(double[] a, double[] b)
        {
            return Mul(a,b).Sum();
        }
    }
}
