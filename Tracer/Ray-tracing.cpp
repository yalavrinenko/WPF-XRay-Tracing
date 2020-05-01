/*
 * main.cpp
 *
 *  Created on: 08.02.2014
 *      Author: cheshire
 */

#include "Trace.hpp"
#include "InputOutput.hpp"
#include "Ray-tracing.hpp"
#include <clocale>
#include "env_builder.hpp"

#ifdef NO_OMP_SUPP
#include <time.h>
double omp_get_wtime() {
    return time(0);
}
#endif

std::string linkedLibraryOutput;
const char *plinkedLibraryOutput;

bool isTerminated;


__lib_spec char const *build_date() {
  return __DATE__;
}

__lib_spec void xrt_terminate() {
  isTerminated = true;
}

void addManualObject(XRTObjectVector &obj, Vec3d mirrorPos, double gridPosition,
                     double size, double angl, std::shared_ptr<tParameters> const &p) {
  double x, y;

  x = -gridPosition * sin(angl);
  if (p->gridLocation == tParameters::GridLocation::AFTER)
    x = -x;

  y = (mirrorPos.y + p->mirrorR) - gridPosition * cos(angl);

  Vec3d posGrid(x, y, 0);
  mirrorPos.y += p->mirrorR;
  Vec3d direction = mirrorPos - posGrid;

  direction = direction / sqrt(dot(direction));

  auto g = std::make_shared<tObjectPlane>(direction, posGrid, p);
  g->type = GRID;
  g->xrt_parameters(p);

  obj.push_back(g);
}

__lib_spec int RayTracing(int argc, char const *argv, ProgressCallback raysGenerated, WaveCallback waveTraced,
                          StdOutCallback stdoutCallback) {

#ifdef _WIN32
  std::setlocale(LC_ALL, "C");
#endif

  isTerminated = false;

  std::shared_ptr<tParameters> p = nullptr;

  double startTime = omp_get_wtime();

  if (argc < 1) {
    printf("No input file!\n");
    return 1;
  } else
    p = std::make_shared<tParameters>(argv);

  XRTSystem object_system(p);


  if (p->objPlaneCount > 0)
    addDumpPlanes(obj, p->objStartPoint, dumpPlaneStart, p->objPlaneCount,
                  p->objPlaneStep, p->objPlaneSizeW, p->objPlaneSizeH, -p->programAngle,
                  tDetectorPlane::IntersectionFilter::OBJECT, p);

  if (p->gridPos > 0 && p->gridType != "none") {
    if (p->gridType == "manual")
      addManualObject(obj, mirror->GetR0(), p->gridPos, p->gridSize, p->programAngle, p);
  }

  infoOut log(p->logFileName.c_str(), stdoutCallback);
  log.logText("XRT Version: DATE [" + std::string(__DATE__) + "], TIME [" + std::string(__TIME__) + "]");
  log.logText("Input File Name = " + string(argv) + "\n");
  p->logVariable(log);
  log.logText("Rmirror\t=\t" + doubleToStr(p->mirrorR));

  log.logScene(mirror, light);

  log.logText("Size\t=\t" + doubleToStr(p->sourceSize));
  log.logText("Dist\t=\t" + doubleToStr(p->sourceDistance));

  if (p->waveLenghtCount <= 0) {
    log.logText("No wave!!!!");
    cout << "No wave!!!" << endl;
    return 1;
  }

  //

  XRayTracingLog xlog{};

  linkedLibraryOutput = std::string("[" + to_string(0) + "/" + to_string(p->waveLenghtCount) + "]:\t");

  xlog.totalWaves = p->waveLenghtCount;
  xlog.currentWaves = 0;

  XRTEnvironment xrt_env;

  for (int w = 0; w < p->waveLenghtCount && !isTerminated; w++) {

    waveInput const &currentWaveLenght = p->waveLenghts[w];
    if (currentWaveLenght.waveLenght <= 0)
      continue;

    log.logText(
        "Set wave lenght\t=\t"
        + doubleToStr(currentWaveLenght.waveLenght));
    log.logText(
        "Set wave dlenght\t=\t"
        + doubleToStr(currentWaveLenght.dwaveLenght));
    log.logText(
        "Set wave intensity\t=\t"
        + doubleToStr(currentWaveLenght.intensity));

    cout << "Wave [lenght/width/intensity]\t=\t[" << currentWaveLenght.waveLenght << "/";
    cout << currentWaveLenght.dwaveLenght << "/";
    cout << currentWaveLenght.intensity << "]" << endl;

    mirror->initRayCounter();
    mirror->setWorkingWave(currentWaveLenght.waveLenght);
    log.logText("Switch working wavelenght to " + std::to_string(currentWaveLenght.waveLenght));

    int rayByIter = p->rayByIter;
    auto totalRays =static_cast<int>(p->rayCount * currentWaveLenght.intensity);
    xlog.linkedLibraryMinorOutput = 0;
    xlog.linkedLibraryTotalOutput = totalRays;

    std::vector<tRay> rays;

    int generatedRay = 0;
    while (generatedRay < totalRays && rayByIter > 0 && !isTerminated) {

      if (rayByIter > (totalRays - generatedRay))
        rayByIter = totalRays - generatedRay;

      rays.resize(rayByIter);
      auto ray_generator = [&light, currentWaveLenght](auto begin, auto end){
        light->generate_rays(begin, end, currentWaveLenght.waveLenght, currentWaveLenght.dwaveLenght);
      };

      xrt_env.generate_rays(rays.begin(), rays.end(), ray_generator);

      if (isTerminated) {
        break;
      }

      xrt_env.trace(rays, obj);

      generatedRay += rays.size();
      xlog.linkedLibraryMinorOutput = generatedRay;

      raysGenerated(xlog);
    }

    double rCast = generatedRay;
    auto rCatch = static_cast<double>(mirror->getCatchRayCount());
    auto rRefl = static_cast<double>(mirror->getReflRayCount());
    double rI = rRefl / rCatch;

    XRayWaveResult xwave{};
    xwave.captured = rCatch;
    xwave.generate = rCast;
    xwave.dlambda = currentWaveLenght.dwaveLenght;
    xwave.lambda = currentWaveLenght.waveLenght;
    xwave.reflected = rRefl;
    xwave.relfectivity = rRefl;
    xwave.intensity = currentWaveLenght.intensity;

    waveTraced(xwave);

    linkedLibraryOutput = "[lambda gen catch relf reflection_stage] =\t"
                          + doubleToStr(currentWaveLenght.waveLenght) + "\t"
                          + doubleToStr(rCast) + "\t" + doubleToStr(rCatch) + "\t"
                          + doubleToStr(rRefl) + "\t" + doubleToStr(rI);

    log.logText(linkedLibraryOutput);

    linkedLibraryOutput =
        std::string("[").append(to_string(w + 1)).append("/").append(to_string(p->waveLenghtCount)).append(
            "]:\t").append(linkedLibraryOutput);

    plinkedLibraryOutput = linkedLibraryOutput.c_str();

    xlog.currentWaves++;

    cout << endl;
  }
  //

  double finishTime = omp_get_wtime();
  cout << "Done!" << endl;
  cout << "Working time:" << "\t" << finishTime - startTime << " sec.";
  log.logText("Working time:\t" + doubleToStr(finishTime - startTime) + " sec.");
  return 0;
}
