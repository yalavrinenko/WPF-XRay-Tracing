using OxyPlot.Wpf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using MathNet.Numerics.LinearAlgebra;
using OxyPlot;

namespace Interface
{
    public class ConfigPlotter
    {
        private Plot m_Canvas;

        public Plot Canvas { get { return m_Canvas; } }

        public ConfigPlotter(Plot canvas)
        {
            m_Canvas = canvas;
        }

        delegate double[] DrawCircle(double a, double b);

        delegate double[] DrawLine(double a, double[] b, double[] c);

        private void PlotCircle(DrawCircle func, Vector<double> x, double R, string color, int linewidth)
        {
            LineSeries ls = new LineSeries()
            {
                Color = OxyPlot.OxyColor.Parse(color).ToColor(),
                StrokeThickness = linewidth
            };
            DataPoint[] p = new DataPoint[x.Count];
            for (int i=0; i<x.Count; ++i)
            {
                var pd = func(x[i], R);
                p[i] = new DataPoint(pd[0], pd[1]);
            }
            ls.ItemsSource = p;
            m_Canvas.Series.Add(ls);

            m_Canvas.InvalidatePlot(true);
        }

        private void PlotFunc(Func<double, double[]> func, Vector<double> x, string color, int linewidth, LineStyle style = LineStyle.Solid)
        {
            LineSeries ls = new LineSeries()
            {
                Color = OxyPlot.OxyColor.Parse(color).ToColor(),
                StrokeThickness = linewidth,
                LineStyle = style
            };
            DataPoint[] p = new DataPoint[x.Count];
            for (int i = 0; i < x.Count; ++i)
            {
                var pd = func(x[i]);
                p[i] = new DataPoint(pd[0], pd[1]);
            }
            ls.ItemsSource = p;
            m_Canvas.Series.Add(ls);
            m_Canvas.InvalidatePlot(true);
        }

        private void PlotPoints(double[] x, double[] y, string color, int linewidth, LineStyle style = LineStyle.Solid)
        {
            LineSeries ls = new LineSeries()
            {
                Color = OxyPlot.OxyColor.Parse(color).ToColor(),
                StrokeThickness = linewidth,
                LineStyle = style
            };
            DataPoint[] p = new DataPoint[x.Length];
            for (int i = 0; i < x.Length; ++i)
            {
                p[i] = new DataPoint(x[i], y[i]);
            }
            ls.ItemsSource = p;
            m_Canvas.Series.Add(ls);
            m_Canvas.InvalidatePlot(true);
        }

        public void DrawConfiguration(SystemConfig self)
        {
            m_Canvas.Series.Clear();

            DrawCircle circle = (alpha, R) => { return new double[] { R * Math.Cos(alpha), R * Math.Sin(alpha) }; };
            // plot crystall;
            var phi_max = Math.Asin(0.5 * self.crystalW / self.crystalR);
            int N = (int)(2.0 * phi_max / 0.001);
            var phis = Vector<double>.Build.Dense(N, i => -phi_max + i * 0.001);
            phis = phis + Math.PI / 2.0;

            PlotCircle(circle, phis, self.crystalR, "#FF0000", 5);

            // plot source;
            /*            elipse = lambda alpha, size, shift: [size[0] * np.cos(alpha) + shift[0], size[1] * np.sin(alpha) + shift[1]];
                        b_phi = self.BraggA * np.pi / 180.0;
                        source_pos = [-self.SrcDist * np.cos(b_phi), -self.SrcDist * np.sin(b_phi) - self.crystalR];
                        figure_ax.plot(*elipse(np.arange(-np.pi, np.pi, 0.001), [self.SrcSizeW, self.SrcSizeH], source_pos),;
                        color = "blue");*/

            // plot detector;
            var b_phi = self.BraggA * Math.PI / 180.0;
            double[] source_pos = { -self.SrcDist * Math.Cos(b_phi), -self.SrcDist * Math.Sin(b_phi) + self.crystalR };

            var detector_center = new double[] { self.DstDist * Math.Cos(b_phi), -self.DstDist * Math.Sin(b_phi) + self.crystalR };

            var detector_norm_line = Extension.Sub(detector_center, source_pos);
            var norm = Extension.vectorLenght(detector_norm_line);
            detector_norm_line = detector_norm_line.Select(x => x / norm).ToArray();

            var detector_half_size = 20.0;
            if (self.waveLimits != null)
                detector_half_size = (Math.Abs(self.waveLimits.max.position) + Math.Abs(self.waveLimits.min.position)) * 1.25;

            N = (int)(2.0 * detector_half_size / 0.1);
            var step = Vector<double>.Build.Dense(N, i => -detector_half_size + i * 0.1);

            var r0 = detector_center;
            var direction = detector_norm_line;

            Func<double, double[]> line = (steps) => { return new double[] { steps * direction[0] + r0[0], steps * direction[1] + r0[1] }; };

            PlotFunc(line, step, "#00FF00", 5);
            // plot object;
            if (self.ObjectExist)
            {
                var object_distance = self.Object.GridPosition;
                double[] object_coord = { ((self.Object.GridLocation == 0) ? -1 : 1) *  object_distance * Math.Cos(b_phi),
                    -object_distance * Math.Sin(b_phi) + self.crystalR };

                double[] object_vector = { -object_coord[0], self.crystalR - object_coord[1] };
                double tmp = object_vector[0];
                double object_vector_norm = Extension.vectorLenght(object_vector);
                object_vector[0] = -object_vector[1] / object_vector_norm;
                object_vector[1] = tmp / object_vector_norm;

                var ObjectN = (int)(self.Object.GridWidth / 0.1);
                var object_t_step = Vector<double>.Build.Dense(ObjectN, i => - self.Object.GridWidth / 2.0 + i*0.1);

                Func<double, double[]> object_line = (steps) => { return new double[] { steps * object_vector[0] + object_coord[0], steps * object_vector[1] + object_coord[1] }; };
                PlotFunc(object_line, object_t_step, "#00FFFF", 3);
            }


            // plot lines;
            var dh = Math.Sqrt(self.crystalR * self.crystalR - self.crystalW * self.crystalW / 4.0);
            double[] cross_point_1 = { -self.crystalW / 2.0, dh };
            double[] cross_point_2 = { self.crystalW / 2.0, dh };

            Func<double, double[], double[], double[]> lpoint = (steps, r0p, directionP) => { return new double[] { steps * directionP[0] + r0p[0], steps * directionP[1] + r0p[1] }; };

            var detector_cross_1 = lpoint((self.waveLimits != null) ? self.waveLimits.max.position : 10, detector_center, detector_norm_line);
            var detector_cross_2 = lpoint((self.waveLimits != null) ? self.waveLimits.min.position : -10, detector_center, detector_norm_line);

            PlotPoints(new double[] { source_pos[0], cross_point_1[0], detector_cross_1[0] },
                new double[] { source_pos[1], cross_point_1[1], detector_cross_1[1] }, "#0000FF", 1);

            PlotPoints(new double[] { source_pos[0], cross_point_2[0], detector_cross_2[0] },
                new double[] { source_pos[1], cross_point_2[1], detector_cross_2[1] }, "#FF0000", 1);

            // plot Roland circle;

            double[] RowlandCenter = {0.0, -self.crystalR / 2.0};
            N = (int) (Math.PI * 2 / 0.001);
            phis = Vector<double>.Build.Dense(N, i => -Math.PI + i * 0.001);
            Func<double, double[]> sh_circle = (alpha) =>
            {
                return new double[]{self.crystalR / 2.0 * Math.Cos(alpha) - RowlandCenter[0],
                self.crystalR / 2.0 * Math.Sin(alpha) - RowlandCenter[1] };
            };

            PlotFunc(sh_circle, phis, "#000000", 2, LineStyle.Dash);


            // plot normal to crystal and start of the system;

            PlotPoints(new double[] { 0.0, 0.0 }, new double[] { self.crystalR, -self.crystalR }, "#000000", 2, LineStyle.Dash);

            m_Canvas.ResetAllAxes();
        }
    }
}
