using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OxyPlot;
using OxyPlot.Wpf;

namespace Interface
{
    /*
    * <oxy:Plot Name="WavePlotCanvas">
                        <oxy:Plot.Axes >
                            <oxy:LinearAxis Position="Bottom" Title="Wavelength"></oxy:LinearAxis>
                            <oxy:LinearAxis Position="Left" Title="Intensity"></oxy:LinearAxis>
                            <oxy:LinearAxis Position="Right" Title="Reflectivity"></oxy:LinearAxis>
                        </oxy:Plot.Axes>
                    </oxy:Plot>
                    */
    public class WavePlotter {

        private Plot m_Model;
        private SystemConfig.WaveLimits m_Limits;
        private LinearBarSeries m_WaveLimitBar;

        public bool FullRange = true;

        public WavePlotter(Plot Canvas)
        {
            m_Model = Canvas;
            m_WaveLimitBar = new LinearBarSeries()
            {
                BarWidth = 0.01,
                StrokeColor = System.Windows.Media.Colors.Red,
                FillColor = System.Windows.Media.Colors.Red,
                StrokeThickness = 5
            };
            m_Model.Series.Add(m_WaveLimitBar);
        }

        public void SetWaveLimits(SystemConfig.WaveLimits WaveLimits)
        {
            m_Limits = WaveLimits;

            m_Model.Series.Remove(m_WaveLimitBar);

            if (FullRange == false)
            {
                m_Model.Axes[0].Minimum = m_Limits.min.lambda * 0.75;
                m_Model.Axes[0].Maximum = m_Limits.max.lambda * 1.25;
            }
            else
            {
                if (m_Model.Axes[0].Minimum > m_Limits.min.lambda * 0.95 || Double.IsNaN(m_Model.Axes[0].Minimum))
                    m_Model.Axes[0].Minimum = m_Limits.min.lambda * 0.95;

                if (m_Model.Axes[0].Maximum < m_Limits.max.lambda * 1.05 || Double.IsNaN(m_Model.Axes[0].Maximum))
                    m_Model.Axes[0].Maximum = m_Limits.max.lambda * 1.05;
            }

            List<DataPoint> limitBar = new List<DataPoint>();
            limitBar.Add(new DataPoint(m_Limits.min.lambda, 1.0));
            limitBar.Add(new DataPoint(m_Limits.max.lambda, 1.0));
            m_WaveLimitBar.ItemsSource = limitBar;

            m_Model.Series.Add(m_WaveLimitBar);
            m_Model.InvalidatePlot(true);
        }

        public void PlotLines(List<Wave> waves)
        {
            m_Model.Series.Clear();
            m_Model.InvalidatePlot(true);

            if (waves.Count != 0)
            {
                var minW = waves.Min(x => x.lambda) * 0.95;
                var maxW = waves.Max(x => x.lambda) * 1.05;

                m_Model.Axes[0].Minimum = minW;
                m_Model.Axes[0].Maximum = maxW;
            }

            if (m_Limits != null)
            {
                SetWaveLimits(m_Limits);
            }
         
            foreach (var groupedWave in waves.GroupBy(x => x.Order))
            {
                var waveSeries = new LinearBarSeries()
                {
                    BarWidth = 0.0001,
                    StrokeThickness = 2
                };

                waveSeries.ItemsSource = groupedWave.Select(x =>
               {
                   var dh = (x.dlambda*1e-3) / 2.0;
                   return new DataPoint(x.lambda, x.intensity);
               }).OrderBy(x=>x.X).ToList();
                
                m_Model.Series.Add(waveSeries);
                m_Model.InvalidatePlot(true);
            }


            var lineSeries = new LineSeries()
            {
                LineStyle = LineStyle.None,
                MarkerFill = OxyColors.Blue.ToColor(),
                MarkerSize = 5,
                MarkerType = MarkerType.Circle
            };
            lineSeries.YAxisKey = "WaveReflectivity";
            lineSeries.XAxisKey = "WaveKey";

            List<DataPoint> pointList = new List<DataPoint>();
            foreach( var wave in waves)
            {
                pointList.Add(new DataPoint(wave.lambda, wave.Efficiency));
            }
            lineSeries.ItemsSource = pointList;
            lineSeries.Items.Refresh();

            m_Model.Series.Add(lineSeries);
            m_Model.InvalidatePlot(true);
        }
    }
}

