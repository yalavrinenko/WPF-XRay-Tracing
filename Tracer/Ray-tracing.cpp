/*
 * main.cpp
 *
 *  Created on: 08.02.2014
 *      Author: cheshire
 */

#include "Ray-tracing.hpp"
#include "InputOutput.hpp"
#include "Trace.hpp"
#include "env_builder.hpp"
#include <clocale>

#ifdef NO_OMP_SUPP
#include <time.h>
double omp_get_wtime() { return time(0); }
#endif

std::string linkedLibraryOutput;
const char *plinkedLibraryOutput;

bool isTerminated;


__lib_spec char const *build_date() { return __DATE__; }

__lib_spec void xrt_terminate() { isTerminated = true; }

void log_start_info(infoOut &log, char const *argv, std::shared_ptr<tParameters> const &p,
                    XRTSystem const &object_system) {
  log.logText("XRT Version: DATE [", __DATE__, "], TIME [", __TIME__, "]");
  log.logText("Input File Name = ", argv, "\n");
  p->logVariable(log);
  log.logText("Rmirror\t=", std::to_string(p->mirrorR));

  log.logScene(object_system.crystal(), object_system.source());

  log.logText("Size\t=", p->sourceSize);
  log.logText("Dist\t=", p->sourceDistance);
}

int trace_single_wavelenght(waveInput current_wave, int rayByIter, int totalRays,
                             XRTSystem &object_system, XRTEnvironment &xrt_env,
                             XRayTracingLog &xlog, ProgressCallback const &raysGenerated) {
  object_system.crystal()->initRayCounter();

  xlog.linkedLibraryMinorOutput = 0;
  xlog.linkedLibraryTotalOutput = totalRays;

  std::vector<tRay> rays;

  auto generatedRay = 0;
  while (generatedRay < totalRays && rayByIter > 0 && !isTerminated) {
    rayByIter = std::min(rayByIter, totalRays - generatedRay);

    rays.resize(rayByIter);
    auto ray_generator = [&object_system, current_wave](auto begin, auto end) {
      object_system.source()->generate_rays(begin, end, current_wave.waveLenght,
                                            current_wave.dwaveLenght);
    };

    xrt_env.generate_rays(rays.begin(), rays.end(), ray_generator);

    if (isTerminated) { break; }

    xrt_env.trace(rays, object_system.objects());

    generatedRay += rays.size();
    xlog.linkedLibraryMinorOutput = generatedRay;

    raysGenerated(xlog);
  }
  return generatedRay;
}

void main_loop(std::shared_ptr<tParameters> const &p, XRTSystem &object_system, infoOut &log,
               ProgressCallback const &raysGenerated, WaveCallback const &waveTraced) {
  XRayTracingLog xlog{};

  xlog.totalWaves = p->waveLenghtCount;
  xlog.currentWaves = 0;

  XRTEnvironment xrt_env;

  for (int w = 0; w < p->waveLenghtCount && !isTerminated; w++) {

    waveInput const &current_wave = p->waveLenghts[w];
    if (current_wave.waveLenght <= 0) continue;

    log.logText("Set wave lenght\t=", current_wave.waveLenght);
    log.logText("Set wave dlenght\t=", current_wave.dwaveLenght);
    log.logText("Set wave intensity\t=", current_wave.intensity);

    auto working_wave = object_system.crystal()->setWorkingWave(current_wave.waveLenght);
    log.logText("Switch working wavelenght to ", working_wave);

    auto total_rays = static_cast<std::size_t>(p->rayCount * current_wave.intensity);
    auto rCast = trace_single_wavelenght(current_wave, p->rayByIter, total_rays, object_system,
                                         xrt_env, xlog, raysGenerated);

    auto rCatch = static_cast<double>(object_system.crystal()->getCatchRayCount());
    auto rRefl = static_cast<double>(object_system.crystal()->getReflRayCount());
    double rI = rRefl / rCatch;

    XRayWaveResult xwave{current_wave.waveLenght, current_wave.dwaveLenght, current_wave.intensity,
                         static_cast<double>(rCast), rCatch, rRefl, rI};

    waveTraced(xwave);

    log.logText("[lambda gen catch relf reflection_stage] =", current_wave.waveLenght,
                rCast, rCatch, rRefl, rI);

    xlog.currentWaves++;

    cout << endl;
  }
}

__lib_spec int RayTracing(int argc, char const *argv, ProgressCallback raysGenerated,
                          WaveCallback waveTraced, StdOutCallback stdoutCallback) {

#ifdef _WIN32
  std::setlocale(LC_ALL, "C");
#endif

  isTerminated = false;

  std::shared_ptr<tParameters> p = nullptr;
  auto startTime = std::chrono::high_resolution_clock::now();

  if (argc < 1)
    return 1;

  p = std::make_shared<tParameters>(argv);

  XRTSystem object_system(p);

  infoOut log(p->logFileName.c_str(), stdoutCallback);
  log_start_info(log, argv, p, object_system);

  main_loop(p, object_system, log, raysGenerated, waveTraced);

  auto finishTime = std::chrono::high_resolution_clock::now();
  auto wtime = std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count();
  log.logText("Working time:", wtime, " sec.");
  return 0;
}
