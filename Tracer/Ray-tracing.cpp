/*
 * main.cpp
 *
 *  Created on: 08.02.2014
 *      Author: cheshire
 */

#include "tSphere.hpp"
#include "tCylinder.hpp"
#include "LightSorce.hpp"
#include "Trace.hpp"
#include "InputOutput.hpp"
#include "Ray-tracing.hpp"
#include "tObject.hpp"
#include <clocale>

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

template<class mirrorClass>
std::shared_ptr<XRTMirror> initMirror(std::shared_ptr<tParameters> const &p) {
  double mirrorYpos = p->sourceDistance * cos(p->programAngle) - p->mirrorR;
  Vec3d mpos(0, mirrorYpos, 0);
  Vec3d mprop(p->mirrorR, p->mirrorTheta, p->mirrorPsi);
  Vec3d mdprop(0, p->dmTh, p->dmPsi);

  auto mirror = std::make_shared<mirrorClass>(mpos, mprop, mdprop, p->mirrorDumpFileName);
  mirror->type = p->mirrorType + 1;
  mirror->xrt_parameters(p);

  return mirror;
}

std::shared_ptr<XRTRaySource>
initRaySource(std::shared_ptr<tParameters> const &p, std::shared_ptr<XRTTargetSurface<>> &&target) {
  double srcXpos = p->sourceDistance * sin(p->programAngle);
  Vec3d spos(-srcXpos, 0, 0);
  return std::make_shared<SphereLight>(spos, p->sourceSize_W, p->sourceSize_H, std::random_device{}(),
                                       std::forward<std::shared_ptr<XRTTargetSurface<>>>(target));
}

void addDumpPlanesObjectLined(XRTObjectVector &obj, double startPoint, Vec3d dir,
                              Vec3d object_r, int count, double step, double sizeW, double sizeH, double angl,
                              tDetectorPlane::IntersectionFilter crossPattern, std::shared_ptr<tParameters> const &p) {
  double h = startPoint;
  for (int i = 0; i < count; i++) {
    double sx = h * sin(angl);
    double sy = h * cos(angl);
    sy = dir.y - sy;

    Vec3d r0(sx, sy, 0);
    Vec3d N = object_r - r0;
    N = N / sqrt(dot(N));

    double nx = N.x * cos(-M_PI / 2) - N.y * sin(-M_PI / 2);
    double ny = N.x * sin(-M_PI / 2) + N.y * cos(-M_PI / 2);
    N.x = nx;
    N.y = ny;

    //string name = p->dumpPlaneName + doubleToStr(h) + ".dmp";
    string name = p->dumpPlaneName + "Detector.dmp";

    auto dp = std::make_shared<tDetectorPlane>(N, r0, sizeW, sizeH, name);
    dp->setCrossPattern(crossPattern);
    dp->type = DUMP_PLANE;
    dp->xrt_parameters(p);

    obj.push_back(dp);
    h += step;
  }
}

void addDumpPlanesSrcLined(XRTObjectVector &obj, double startPoint, Vec3d dir,
                           Vec3d srcDir, int count, double step, double sizeW, double sizeH, double angl,
                           tDetectorPlane::IntersectionFilter crossPattern, std::shared_ptr<tParameters> const &p) {
  double h = startPoint;
  for (int i = 0; i < count; i++) {
    double sx = h * sin(angl);
    double sy = h * cos(angl);
    sy = dir.y - sy;

    Vec3d r0(sx, sy, 0);
    Vec3d N = srcDir - r0;
    N = N / sqrt(dot(N));

    double nx = N.x * cos(-M_PI / 2) - N.y * sin(-M_PI / 2);
    double ny = N.x * sin(-M_PI / 2) + N.y * cos(-M_PI / 2);
    N.x = nx;
    N.y = ny;

    //string name = p->dumpPlaneName + doubleToStr(h) + ".dmp";
    string name = p->dumpPlaneName + "Detector.dmp";

    auto dp = std::make_shared<tDetectorPlane>(N, r0, sizeW, sizeH, name);
    dp->setCrossPattern(crossPattern);
    dp->type = DUMP_PLANE;
    dp->xrt_parameters(p);

    obj.push_back(dp);
    h += step;
  }
}

void addDumpPlanesR2Lined(XRTObjectVector &obj, double startPoint, Vec3d dir,
                          Vec3d mirror_r, int count, double step, double sizeW, double sizeH, double angl,
                          tDetectorPlane::IntersectionFilter crossPattern, std::shared_ptr<tParameters> const &p) {
  double h = startPoint;
  for (int i = 0; i < count; i++) {
    double sx = h * sin(angl);
    double sy = h * cos(angl);
    sy = dir.y - sy;

    Vec3d r0(sx, sy, 0);

    Vec3d N = mirror_r - r0;
    N = N / sqrt(dot(N));

    double nx = N.x * cos(-M_PI / 2) - N.y * sin(-M_PI / 2);
    double ny = N.x * sin(-M_PI / 2) + N.y * cos(-M_PI / 2);
    N.x = nx;
    N.y = ny;

    //string name = p->dumpPlaneName + doubleToStr(h) + ".dmp";
    string name = p->dumpPlaneName + "Detector.dmp";

    auto dp = std::make_shared<tDetectorPlane>(N, r0, sizeW, sizeH, name);
    dp->setCrossPattern(crossPattern);
    dp->type = DUMP_PLANE;
    dp->xrt_parameters(p);

    obj.push_back(dp);
    h += step;
  }
}

void addDumpPlanes(XRTObjectVector &obj, double startPoint, Vec3d dir,
                   int count, double step, double sizeW, double sizeH, double angl,
                   tDetectorPlane::IntersectionFilter crossPattern, std::shared_ptr<tParameters> const &p) {
  double h = startPoint;
  for (int i = 0; i < count; i++) {
    double sx = h * sin(angl);
    double sy = h * cos(angl);
    sy = dir.y - sy;

    Vec3d r0(sx, sy, 0);
    Vec3d N(sx, sy, 0);
    N = dir - N;
    N = N / sqrt(dot(N));

    //string name = p->dumpPlaneName + doubleToStr(h) + ".dmp";
#ifdef DEUBG_MODE
    string name = p->dumpPlaneName + std::to_string(h) + ".dmp";
#else
    string name = p->dumpPlaneName + "Detector.dmp";
#endif

    auto dp = std::make_shared<tDetectorPlane>(N, r0, sizeW, sizeH, name);
    dp->setCrossPattern(crossPattern);
    dp->type = DUMP_PLANE;
    dp->xrt_parameters(p);

    obj.push_back(dp);
    h += step;
  }
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

  XRTObjectVector obj;
  std::shared_ptr<tParameters> p = nullptr;

  double startTime = omp_get_wtime();

  if (argc < 1) {
    printf("No input file!\n");
    return 1;
  } else
    p = std::make_shared<tParameters>(argv);

  std::shared_ptr<XRTMirror> mirror = nullptr;
  if (p->mirrorType == MIRROR_SPHERE)
    mirror = initMirror<tSphere>(p);
  if (p->mirrorType == MIRROR_CYLINDER)
    mirror = initMirror<tCylinder>(p);

  obj.emplace_back(mirror);

  auto light = initRaySource(p, mirror);

  Vec3d dumpPlaneStart = mirror->GetR0();

  dumpPlaneStart.y += p->mirrorR;

  if (p->planeCount > 0) {
    if (p->dumpDirection == "Mirror")
      addDumpPlanes(obj, p->startPoint, dumpPlaneStart, p->planeCount,
                    p->planeStep, p->planeSizeW, p->planeSizeH, p->programAngle,
                    tDetectorPlane::IntersectionFilter::IMAGE, p);
    else {
      //addDumpPlanesR2Lined(obj, p->startPoint, dumpPlaneStart,
      //mirror->GetR0(), p->planeCount, p->planeStep, p->planeSizeW, p->planeSizeH,
      //p->programAngle, tDetectorPlane::IntersectionFilter::IMAGE, p);
      addDumpPlanesObjectLined(obj, p->startPoint, dumpPlaneStart,
                               mirror->GetR0(), p->planeCount, p->planeStep, p->planeSizeW, p->planeSizeH,
                               p->programAngle, tDetectorPlane::IntersectionFilter::IMAGE, p);
    }
  }

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

  cout << "Done!\n\tCleaning memory..." << endl;
  log.logText("Done!\n\tCleaning memory...");
  try {
    obj.clear();
  }
  catch (...) {
    cout << "Catch something strange!" << endl;
    log.logText("Catch something strange!");
  }

  cout << "\tClean!" << endl;
  log.logText("\tClean!");

  double finishTime = omp_get_wtime();
  cout << "Working time:" << "\t" << finishTime - startTime << " sec.";
  log.logText("Working time:\t" + doubleToStr(finishTime - startTime) + " sec.");
  cout << endl << "End" << endl;
  return 0;
}
