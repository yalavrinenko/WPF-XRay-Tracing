using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Interface
{
    public class DataBaseWavelength
    {
        public string name { get; set; }
        public double lambda { get; set; }
        public double dlambda { get; set; }
        public double intensity { get; set; }

        public DataBaseWavelength(DataBaseWavelength w)
        {
            if (w == null)
            {
                return;
            }
            name = w.name;
            lambda = Math.Round(w.lambda,5);
            dlambda = w.dlambda;
            intensity = w.intensity;
        }

        public DataBaseWavelength() { }
        public override string ToString()
        {
            return name + " " + lambda.ToString();
        }
    }
    public class WaveLengthLoader
    {
        private List<DataBaseWavelength> m_WaveDatabase;
        private string m_DBName;

        public string DataBasePath { get { return m_DBName; } set { m_DBName = value; } }
        public WaveLengthLoader(string DBFile)
        {
            m_DBName = DBFile;
            ReadDatabase();
        }

        public WaveLengthLoader(WaveLengthLoader loader)
        {
            m_DBName = loader.m_DBName;
            ReadDatabase();
        }

        public WaveLengthLoader()
        {
        }

        public List<DataBaseWavelength> Search(string pattern)
        {
            List<DataBaseWavelength> res = new List<DataBaseWavelength>();
            foreach (var p in pattern.Split(';'))
            {
                var result = m_WaveDatabase.Where(x => x.ToString().Contains(p)).ToList();
                res.AddRange(result);
            }
            return res;
        }
        private void ReadDatabase()
        {
            StreamReader sr = null;

            try
            {
                sr = new StreamReader(m_DBName);
            }
            catch (Exception e)
            {
                Logger.Error(e.ToString() + "\n" + e.StackTrace);
                return;
            }

            m_WaveDatabase = new List<DataBaseWavelength>();

            while (!sr.EndOfStream)
            {
                var readedLine = sr.ReadLine();
                var values = readedLine.Split('\t');
                try
                {
                    m_WaveDatabase.Add(new DataBaseWavelength()
                    {
                        name = String.Format("{0} {1} {2}", values[0], values[1], values[2]),
                        lambda = Math.Round(Double.Parse(values[3]), 5),
                        dlambda = Double.Parse(values[4]),
                        intensity = Double.Parse(values[5]) / 10.0
                    });
                }
                catch (Exception e)
                {
                    Logger.Warning("Read db warning. " + readedLine);
                }
            }
            sr.Close();
        }
    }
}
