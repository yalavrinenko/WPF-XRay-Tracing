using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Interface
{
    public class Wave: DataBaseWavelength
    {
        public bool isZeroWave { get; set; } = false;
        public int Order { get; set; } = 1;
        public double Emited { get; set; } = 0;
        public double Reflected { get; set; } = 0;
        public double Efficiency { get; set; } = 0;
        public double XCoord { get; set; } = Double.NaN;
        public double Magnification { get; set; } = Double.NaN;
        public double FWHM { get; set; } = Double.NaN;

        public Detector.DispersionCurve Curve { get; set; } = null;
        public Wave(DataBaseWavelength w): base(w)
        {

        }
        public void Clear()
        {
            Emited = 0;
            Reflected = Efficiency = 0;
            XCoord = Magnification = FWHM = Double.NaN;
            Curve = null;
        }
    }
    public class WaveLenghts
    {
        private List<Wave> m_Waves;

        public List<Wave> Wave { get { return m_Waves; } }

        public event EventHandler onWaveAdd;
        public event EventHandler onWaveChange;
        public WaveLenghts()
        {
            m_Waves = new List<Interface.Wave>();
        }

        public Wave[] GetWaveInRagne(SystemConfig.WaveLimits limits)
        {
            for (int i = 0; i < m_Waves.Count; ++i)
                m_Waves[i].Clear();

            return m_Waves.Where(x => (limits.min.lambda * 0.95 < x.lambda && x.lambda < limits.max.lambda * 1.05)).ToArray();
        }
        public void Add (DataBaseWavelength dwave)
        {
            m_Waves.Add(new Interface.Wave(dwave));
            onWaveAdd?.Invoke(this, new EventArgs());
        }

        public void Add(DataBaseWavelength dwave, int order)
        {
            m_Waves.Add(new Interface.Wave(dwave) { Order = order });
            onWaveAdd?.Invoke(this, new EventArgs());
        }

        public void UpdateWave(Wave newWave)
        {
            int index = m_Waves.FindIndex(x => x.lambda == newWave.lambda && x.intensity == newWave.intensity);
            if (index != -1)
            {
                m_Waves[index].Efficiency = newWave.Efficiency;
                m_Waves[index].Emited = newWave.Emited;
                m_Waves[index].Reflected = newWave.Reflected;

                onWaveChange?.Invoke(this, new EventArgs());
            }
        }

        public void AddFeatures(Detector.LineFeatures feature, Wave wave)
        {
            int index = m_Waves.FindIndex(x => x.lambda == wave.lambda && x.intensity == wave.intensity);
            if (index != -1)
            {
                m_Waves[index].FWHM = feature.FWHM;
                m_Waves[index].XCoord = feature.XCoord;
                m_Waves[index].Magnification = feature.Size;
                m_Waves[index].Curve = feature.Curve;

                onWaveChange?.Invoke(this, new EventArgs());
            }
        }

        public Wave GetZeroWave()
        {
            var zeroWave = m_Waves.Find(w => w.isZeroWave == true);
            if (zeroWave == null) {
                zeroWave = m_Waves.Find(w => w.Efficiency > 0);
                if (zeroWave != null)
                    m_Waves.Find(w => w == zeroWave).isZeroWave = true;
            }
            return zeroWave;
        }

        public void GenerateAdditionalWaves(List<int> orders, SystemConfig self)
        {
            var max_crystal_2d = self.CrystalProps.crystal2d.Max();

            List<Tuple<DataBaseWavelength, int>> waveToAdd = new List<Tuple<DataBaseWavelength, int>>();

            foreach (int order in orders.Where(x => x != 0 ))
            {
                foreach (var wave in m_Waves.Where(x => x.name.Contains("_order_") == false))
                {
                    if (wave.Order != order) {
                        var newLine = new DataBaseWavelength()
                        {
                            name = wave.name + "_order_" + order.ToString(),
                            lambda = wave.lambda * order / wave.Order * (
                                self.CrystalProps.crystal2d[order - 1] / self.CrystalProps.crystal2d[wave.Order - 1]),
                            intensity = wave.intensity * (double)(wave.Order) / (double)order
                        };
                        newLine.dlambda = newLine.lambda;

                        if (m_Waves.Find(x => x.name == newLine.name) == null && newLine.lambda <= max_crystal_2d)
                        {
                            waveToAdd.Add(new Tuple<DataBaseWavelength, int>(newLine, order));
                        }
                    }
                }
            }

            foreach (var newLine in waveToAdd)
            {
                this.Add(newLine.Item1, newLine.Item2);
            }
        }
        
        public void RemoveAdditionalWaves()
        {
            m_Waves.RemoveAll(x => x.name.Contains("_order_"));
            onWaveChange?.Invoke(this, new EventArgs());
        }

        public void Del(Wave dwave)
        {
            m_Waves.Remove(dwave);
            onWaveAdd?.Invoke(this, new EventArgs());
        }

        public void Clear()
        {
            m_Waves.Clear();
            onWaveAdd?.Invoke(this, new EventArgs());
        }
    }
}
