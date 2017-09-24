using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Interface
{
    public static class Averaging
    {
        public static Wave[] GenerateWaveInRangeUniform(double LowLimit, double HiLimit, uint Count)
        {
            Wave[] generated = new Wave[Count];
            double WaveStep = (HiLimit - LowLimit) / Count;
            for (uint index = 0; index < Count; ++index)
            {
                var wavel = LowLimit + index * WaveStep;
                DataBaseWavelength dbWave = new DataBaseWavelength()
                {
                    lambda = wavel,
                    dlambda = wavel,
                    intensity = 1.0,
                    name = "Line " + wavel.ToString("#.00")
                };
                generated[index] = new Wave(dbWave);
            }
            return generated;
        }
    }
}
