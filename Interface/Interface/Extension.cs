using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

namespace Interface
{
    public static class Extension
    {
        public class Range
        {
            public double VMin;
            public double VMax;
        }
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

        public static void SaveTiff(string path, Int16[] pixels, int w, int h)
        {
            int width = w;
            int height = h;
            int stride = w * sizeof(Int16);

            // Define the image palette
            BitmapPalette myPalette = BitmapPalettes.BlackAndWhite;

            // Creates a new empty image with the pre-defined palette

            BitmapSource image = BitmapSource.Create(
                width,
                height,
                100,
                100,
                System.Windows.Media.PixelFormats.Gray16,
                myPalette,
                pixels,
                stride);

            System.IO.FileStream stream = new System.IO.FileStream(path, System.IO.FileMode.Create);
            TiffBitmapEncoder encoder = new TiffBitmapEncoder();
            TextBlock myTextBlock = new TextBlock();
            myTextBlock.Text = "Codec Author is: " + encoder.CodecInfo.Author.ToString();
            encoder.Compression = TiffCompressOption.None;
            encoder.Frames.Add(BitmapFrame.Create(image));
            encoder.Save(stream);
            stream.Close();
        }

        public static double[,] Histogram2D(IEnumerable<double> xvalue, IEnumerable<double> yvalue, Range xrange, Range yrange, double step)
        {
            long M = (long)Math.Ceiling(Math.Abs(xrange.VMax - xrange.VMin) / step);
            long N = (long)Math.Ceiling(Math.Abs(yrange.VMax - yrange.VMin) / step) + 2;

            double[,] hist;
            try
            {
                hist = new double[M, N];
            }
            catch(Exception exc)
            {
                System.Windows.MessageBox.Show("Detector cell size to small! Changed to default size 0.025.", "Detector cell size error", System.Windows.MessageBoxButton.OK, System.Windows.MessageBoxImage.Error);
                return Histogram2D(xvalue, yvalue, xrange, yrange, 0.025);
            }

            var InRegion = Enumerable.Zip(xvalue, yvalue, (x, y) => new { x, y })
                .Where(v => (xrange.VMin <= v.x && v.x <= xrange.VMax) && (yrange.VMin <= v.y && v.y <= yrange.VMax))
                .Select( v => new { x = v.x - xrange.VMin, y = v.y - yrange.VMin });

            foreach (var p in InRegion)
            {
                long xindex = (long)(p.x / step);
                long yindex = (long)(p.y / step) + 1;
                ++hist[xindex, yindex];
            }

            return hist;
        }

        public static long[] Histogram(IEnumerable<double> xvalue, Range xrange, double step)
        {
            if (xvalue.Count() == 0)
                return new long[1]{0};

            ulong N = (ulong)Math.Ceiling(Math.Abs(xrange.VMax - xrange.VMin) / step);
            long[] hist = new long[N];

            var InRegion = xvalue.Where(x => xrange.VMin <= x && x <= xrange.VMax).Select(x => x - xrange.VMin);
            foreach (var x in InRegion)
            {
                long index = (long) (x / step);
                ++hist[index];
            }

            return hist;
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
            double[] c = { a[0] - b[0], a[1] - b[1] };
            return c;
        }

        public static double[] Mul(double[] a, double[] b)
        {
            double[] c = { a[0] * b[0], a[1] * b[1] };
            return c;
        }

        public static double dot(double[] a, double[] b)
        {
            return Mul(a, b).Sum();
        }
    }
}
