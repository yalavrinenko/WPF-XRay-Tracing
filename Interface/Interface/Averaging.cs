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
            var index = 0;
            for (double wavel = LowLimit; wavel < HiLimit; wavel += WaveStep)
            {
                DataBaseWavelength dbWave = new DataBaseWavelength()
                {
                    lambda = wavel,
                    dlambda = wavel,
                    intensity = 1.0,
                    name = "Line " + wavel.ToString("#.00")
                };
                generated[index] = new Wave(dbWave);
                ++index;
            }
            return generated;
        }
    }
}
