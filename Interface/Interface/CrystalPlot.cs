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
    public class CrystalPlot
    {
        private PlotView m_Canvas;
        private List<ScatterPoint> m_Points;

        private class CrystalPoint
        {
            public double x, y, z;
            public double color;
        }

        public List<ScatterPoint> Points { get { return m_Points; } }

        double minColor;
        double maxColor;

        public CrystalPlot(PlotView canvas)
        {
            m_Canvas = canvas;
        }

        public void ReadCrystalPlane(string fileName)
        {
            var lines = File.ReadLines(fileName);
            m_Points = new List<ScatterPoint>();

            minColor = Double.MaxValue;
            maxColor = 0;
            foreach (var line in lines.Skip(6))
            {
                var coords = line.Split('\t');
                CrystalPoint cp = new CrystalPoint()
                {
                    x = double.Parse(coords[0]),
                    y = double.Parse(coords[1]),
                    z = double.Parse(coords[2]),
                    color= double.Parse(coords[3]),
                };

                m_Points.Add(new ScatterPoint(cp.x, cp.z, 4, cp.color));
                if (cp.color < minColor)
                    minColor = cp.color;
                if (cp.color > maxColor)
                    maxColor = cp.color;
            }
        }

        public void PlotMirror(double w, double h)
        {
            var model = new PlotModel();
            model.PlotType = PlotType.Cartesian;

            OxyPlot.Series.ScatterSeries sser = new OxyPlot.Series.ScatterSeries()
            {
                MarkerType = MarkerType.Circle,
            };
            
            sser.ItemsSource = m_Points;

            model.Series.Add(sser);
            model.Axes.Add(new OxyPlot.Axes.LinearAxis { Position = AxisPosition.Bottom, Maximum = w / 2.0, Minimum = -w / 2.0, Title="x, [mm]" });
            model.Axes.Add(new OxyPlot.Axes.LinearAxis { Position = AxisPosition.Left, Maximum = h / 2.0, Minimum = -h / 2.0, Title="y, [mm]" });
            model.Axes.Add(new OxyPlot.Axes.LinearColorAxis {
                Position = AxisPosition.Right,
                Palette = OxyPalettes.Rainbow(200),
            });

            m_Canvas.Model = model;
            m_Canvas.InvalidatePlot(true);
        }
    }
}
