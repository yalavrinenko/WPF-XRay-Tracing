using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Interface
{
    public class AdditionalApproximation
    {
        public class ApproxymationCurve : Detector.DispersionCurve
        {
            public override string ToString()
            {
                if (Math.Abs(a) >= 1e-4 && Math.Abs(b) >= 1e-4 && Math.Abs(c) >= 1e-4)
                    return (isExist) ? String.Format("F(wavelenght=X) = {0}*X^2 + {1}*X + {2}", Extension.mstr(a), Extension.mstr(b), Extension.mstr(c)) : "Unable to construct curve";
                else
                    return (isExist) ? String.Format("F(wavelenght=X) = {0}*X^2 + {1}*X + {2}", b, b, c) : "Unable to construct curve";
            }
        }
        public ApproxymationCurve m_EfficiencyFit { get; private set; }
        public ApproxymationCurve m_MagnificationFit { get; private set; }
        public class ApproximatedPoint : Wave
        {
            public double Approx_Magnification { get; set; } = 0;
            public double Approx_Efficiency { get; set; } = 0;

            public ApproximatedPoint(Wave w) : base(w)
            {
                isZeroWave = w.isZeroWave;
                Order = w.Order;
                Emited = w.Emited;
                Reflected = w.Reflected;
                Efficiency = w.Efficiency;
                XCoord = w.XCoord;
                Magnification = w.Magnification;
                FWHM = w.FWHM;
            }
        }

        public void MakeFitMagnification(Wave[] waves)
        {
            try
            {
                var fitCoeff = MathNet.Numerics.Fit.Polynomial(waves.Where(w => !double.IsNaN(w.Magnification)).Select(w => w.lambda).ToArray(),
                                                               waves.Where(w => !double.IsNaN(w.Magnification)).Select(w => w.Magnification).ToArray(), 2);
                m_MagnificationFit = new ApproxymationCurve()
                {
                    isExist = true,
                    a = fitCoeff[2],
                    b = fitCoeff[1],
                    c = fitCoeff[0]
                };
            }
            catch
            {
                m_MagnificationFit = new ApproxymationCurve()
                {
                    isExist = false
                };
            }
        }

        public void MakeFitEfficiency(Wave[] waves)
        {
            try
            {
                var fitCoeff = MathNet.Numerics.Fit.Polynomial(waves.Where(w => w.Efficiency != 0).Select(w => w.lambda).ToArray(),
                                                               waves.Where(w => w.Efficiency != 0).Select(w => w.Efficiency).ToArray(), 2);
                m_EfficiencyFit = new ApproxymationCurve()
                {
                    isExist = true,
                    a = fitCoeff[2],
                    b = fitCoeff[1],
                    c = fitCoeff[0]
                };
            }
            catch
            {
                m_EfficiencyFit = new ApproxymationCurve()
                {
                    isExist = false
                };
            }
        }

        public ApproximatedPoint[] GetApproximatedPoint(Wave[] in_wave)
        {
            return in_wave.Select(w => new ApproximatedPoint(w)
            {
                Approx_Efficiency = m_EfficiencyFit.Evaluate(w.lambda),
                Approx_Magnification = m_MagnificationFit.Evaluate(w.lambda)
            }).ToArray();
        }
    }

    public partial class MainWindow
    {
        private const int WAVE_COUNT = 40;
        private AdditionalApproximation m_AdditionalApproximation { get; set; } = null;
        private AdditionalApproximation.ApproximatedPoint[] m_AproximatedWavePoints { get; set; } = null;

        private void SetLinesMagnification(Wave[] WaveList, Detector a_Detector)
        {
            foreach (var wave in WaveList)
            {
                var feature = a_Detector.GetLineFeatures(wave);

                if (feature == null)
                    continue;

                feature.Size /= m_SysConf.SrcSizeH;

                wave.FWHM = feature.FWHM;
                wave.Magnification = feature.Size;
            }
        }

        private async Task<bool> ProvideAdditionalApproximation(Wave[] waves)
        {
            m_AdditionalApproximation = new AdditionalApproximation();
            m_AproximatedWavePoints = null;

            List<Wave> fit_waves = waves.ToList();
            if (m_SysConf.UseBetterStatistic == true)
            {
                var waveSet = Averaging.GenerateWaveInRangeUniform(m_SysConf.waveLimits.min.lambda, m_SysConf.waveLimits.max.lambda, WAVE_COUNT);

                Computation computation_engine = null;


                computation_engine = computation_engine = new Computation(ref m_SysConf, waveSet);

                computation_engine.Title = "XRay-tracing. Additional lines.";
                computation_engine.WaveProcessed += new EventHandler<Wave>((obj, wave) =>
                {
                    var index = -1;
                    for (int i = 0; i < waveSet.Length; ++i)
                    {
                        if (waveSet[i].lambda == wave.lambda && waveSet[i].intensity == wave.intensity)
                        {
                            index = i;
                            break;
                        }
                    }
                    if (index < waveSet.Length && index >= 0)
                    {
                        waveSet[index].Efficiency = wave.Efficiency;
                        waveSet[index].Emited = wave.Emited;
                        waveSet[index].Reflected = wave.Reflected;
                    }
                });

                this.IsEnabled = false;

                computation_engine.Show();

                var res = await computation_engine.StartXRayTracing();

                computation_engine.Close();

                this.IsEnabled = true;

                string detectorFile = "results/Order_0/Detector.dmp";
                var a_Detector = new Detector();
                await Task.Factory.StartNew(() => a_Detector.ReadDetectorPlane(detectorFile));

                await Task.Factory.StartNew(() => SetLinesMagnification(waveSet, a_Detector));

                fit_waves.AddRange(waveSet);
            }

            m_AdditionalApproximation.MakeFitEfficiency(fit_waves.ToArray());
            m_AdditionalApproximation.MakeFitMagnification(fit_waves.ToArray());

            m_AproximatedWavePoints = m_AdditionalApproximation.GetApproximatedPoint(waves);

            return true;
        }

    }
}
