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
#include "tMirror.hpp"
#include "Ray-tracing.hpp"
#include "Object.hpp"
#include <clocale>
#include "tObject.hpp"

#define DEBUGMSG(A) cout << A << endl

#ifdef NO_OMP_SUPP
#include <time.h>
double omp_get_wtime() {
	return time(0);
}
#endif

using namespace std;

string linkedLibraryOutput;
const char* plinkedLibraryOutput;

bool isTerminated;

__lib_spec void terminate() {
	isTerminated = true;
}

template <class mirrorClass>
std::shared_ptr<mirrorClass> initScene(XRTObjectVector &obj, SphereLight **slight, tParameters *p) {
	double mirrorYpos = p->sourceDistance * cos(p->programAngle) - p->mirrorR;
	double srcXpos = p->sourceDistance * sin(p->programAngle);

	Vec3d spos(-srcXpos, 0, 0);
	Vec3d sdir(srcXpos, mirrorYpos + p->mirrorR, 0);

	*slight = new SphereLight(spos, sdir, p->aperture, p->sourceSize_W, p->sourceSize_H, p->breggAngle);

	if (p->src_type == SphereLight::cylindricType) {
		(*slight)->setCylinricType(p->H, p->orientation);
	}

	Vec3d mpos(0, mirrorYpos, 0);
	Vec3d mprop(p->mirrorR, p->mirrorTheta, p->mirrorPsi);
	Vec3d mdprop(0, p->dmTh, p->dmPsi);

	auto mirror = std::make_shared<mirrorClass>(mpos, mprop, mdprop, p->mirrorDumpFileName);
	mirror->setDistrFunction(p);
	mirror->type = p->mirrorType + 1;

	obj.push_back(mirror);

	return mirror;
}

void addDumpPlanesSrcLined(XRTObjectVector &obj, double startPoint, Vec3d dir,
	Vec3d srcDir, int count, double step, double sizeW, double sizeH, double angl,
	double crossPattern, tParameters *p) {
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

		auto dp = std::make_shared<tDumpPlane>(N, r0, sizeW, sizeH, (char*)name.c_str());
		dp->setCrossPattern(crossPattern);
		dp->type = DUMP_PLANE;
		obj.push_back(dp);
		h += step;
	}
}

void addDumpPlanes(XRTObjectVector &obj, double startPoint, Vec3d dir,
	int count, double step, double sizeW, double sizeH, double angl, double crossPattern, tParameters *p) {
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

		auto dp = std::make_shared<tDumpPlane>(N, r0, sizeW, sizeH, (char*)name.c_str());
		dp->setCrossPattern(crossPattern);
		dp->type = DUMP_PLANE;
		obj.push_back(dp);
		h += step;
	}
}

void addGrid(XRTObjectVector &obj, Vec3d mirrorPos, double gridPosition,
	double size, double angl, tParameters *p) {
	double x, y;
	x = -gridPosition * sin(angl);
	y = (mirrorPos.y + p->mirrorR) - gridPosition * cos(angl);

	Vec3d posGrid(x, y, 0);
	mirrorPos.y += p->mirrorR;
	Vec3d direction = mirrorPos - posGrid;

	direction = direction / sqrt(dot(direction));

	auto g = std::make_shared<tGrid>(direction, posGrid, size, regularMesh);

	g->type = GRID;

	obj.push_back(g);
}

void addCollimator(XRTObjectVector &obj, Vec3d mirrorPos, double gridPosition,
	double size, double angl, tParameters *p) {
	double x, y;
	x = -gridPosition * sin(angl);
	y = (mirrorPos.y + p->mirrorR) - gridPosition * cos(angl);

	Vec3d posGrid(x, y, 0);
	mirrorPos.y += p->mirrorR;
	Vec3d direction = mirrorPos - posGrid;

	direction = direction / sqrt(dot(direction));

	auto g = std::make_shared<tGrid>(direction, posGrid, size, collimator);
	g->type = GRID;

	obj.push_back(g);
}

void addManualObject(XRTObjectVector &obj, Vec3d mirrorPos, double gridPosition,
                   double size, double angl, tParameters *p) {
    double x, y;

    x = -gridPosition * sin(angl);
    if (p->gridLocation == tParameters::GridLocation::AFTER)
        x = -x;

    y = (mirrorPos.y + p->mirrorR) - gridPosition * cos(angl);

    Vec3d posGrid(x, y, 0);
    mirrorPos.y += p->mirrorR;
    Vec3d direction = mirrorPos - posGrid;

    direction = direction / sqrt(dot(direction));

    auto g = std::make_shared<tObjectPlane>(direction, posGrid, Vec2d{p->gridWidth, p->gridHeight}, p->gridMap);
    g->type = GRID;

    obj.push_back(g);
}

__lib_spec int RayTracing(int argc, char const* argv, ProgressCallback raysGenerated, WaveCallback waveTraced,
                          StdOutCallback stdoutCallback){

#ifdef _WIN32
	std::setlocale(LC_ALL, "C");
#endif

	isTerminated = false;

	XRTObjectVector obj;
	SphereLight *light = nullptr;
	tParameters *p = nullptr;

	double startTime = omp_get_wtime();

	if (argc < 1) {
		printf("No input file!\n");
		return 1;
	}
	else
		p = new tParameters(argv);

	std::shared_ptr<XRTMirror> mirror = nullptr;
	if (p->mirrorType == MIRROR_SPHERE)
		mirror = initScene<tSphere>(obj, &light, p);
	if (p->mirrorType == MIRROR_CYLINDER)
		mirror = initScene<tCylinder>(obj, &light, p);


	Vec3d dumpPlaneStart = mirror->GetR0();

	dumpPlaneStart.y += p->mirrorR;

	if (p->planeCount > 0) {
		if (p->dumpDirection == "Mirror")
			addDumpPlanes(obj, p->startPoint, dumpPlaneStart, p->planeCount,
				p->planeStep, p->planeSizeW, p->planeSizeH, p->programAngle, tDumpPlane::IMAGE, p);
		else
			addDumpPlanesSrcLined(obj, p->startPoint, dumpPlaneStart,
				light->position, p->planeCount, p->planeStep, p->planeSizeW, p->planeSizeH,
				p->programAngle, tDumpPlane::IMAGE, p);
	}

	if (p->objPlaneCount > 0)
		addDumpPlanes(obj, p->objStartPoint, dumpPlaneStart, p->objPlaneCount,
			p->objPlaneStep, p->objPlaneSizeW, p->objPlaneSizeH, -p->programAngle, tDumpPlane::OBJECT, p);

	if (p->gridPos > 0 && p->gridType != "none") {
		if (p->gridType == "mesh")
			addGrid(obj, mirror->GetR0(), p->gridPos, p->gridSize, p->programAngle, p);
		if (p->gridType == "slit")
			addCollimator(obj, mirror->GetR0(), p->gridPos, p->gridSize, p->programAngle, p);
        if (p->gridType == "manual")
            addManualObject(obj, mirror->GetR0(), p->gridPos, p->gridSize, p->programAngle, p);

	}

	infoOut log(p->logFileName.c_str(), stdoutCallback);
	log.logText("XRT Version: DATE [" + std::string(__DATE__)+"], TIME [" + std::string(__TIME__) + "]");
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

	XRayTracingLog xlog;

	linkedLibraryOutput = std::string("[" + to_string(0) + "/" + to_string(p->waveLenghtCount) + "]:\t");

	xlog.totalWaves = p->waveLenghtCount;
	xlog.currentWaves = 0;

	for (int w = 0; w < p->waveLenghtCount && !isTerminated; w++) {

		waveInput currentWaveLenght = p->waveLenghts[w];
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

		int generatedRay = 0;

		mirror->initRayCounter();
		mirror->setWorkingWave(currentWaveLenght.waveLenght);
		log.logText("Switch working wavelenght to"+std::to_string(currentWaveLenght.waveLenght));

		int rayByIter = p->rayByIter;
		xlog.linkedLibraryMinorOutput = 0;
		xlog.linkedLibraryTotalOutput = p->rayCount * currentWaveLenght.intensity;

		while (generatedRay < p->rayCount * currentWaveLenght.intensity && rayByIter > 0 && !isTerminated) {

			if (rayByIter > (p->rayCount * currentWaveLenght.intensity - generatedRay))
				rayByIter = p->rayCount * currentWaveLenght.intensity - generatedRay;

			tRay *ray = light->GenerateRays(currentWaveLenght.waveLenght,
				currentWaveLenght.dwaveLenght, rayByIter);

			if (isTerminated) {
				delete[] ray;
				break;
			}

			rayTrace(ray, rayByIter, obj, obj.size());

			delete[] ray;
			generatedRay += rayByIter;
			xlog.linkedLibraryMinorOutput = generatedRay;

			raysGenerated(xlog);
		}

		double rCast = generatedRay;
		double rCatch = mirror->getCatchRayCount();
		double rRefl = mirror->getReflRayCount();
		double rI = rRefl / rCatch;

		XRayWaveResult xwave;
		xwave.captured = rCatch;
		xwave.generate = rCast;
		xwave.dlambda = currentWaveLenght.dwaveLenght;
		xwave.lambda = currentWaveLenght.waveLenght;
		xwave.reflected = rRefl;
		xwave.relfectivity = rRefl;
		xwave.intensity = currentWaveLenght.intensity;

		waveTraced(xwave);

		printf("===>\t[%d/%d]\t%10.6f\t%10.6f\t%10.6f\t%10.6f\t%10.6f\n", w + 1, p->waveLenghtCount, currentWaveLenght.waveLenght,
			rCast, rCatch, rRefl, rI);

		linkedLibraryOutput = "[lambda gen catch relf I]=\t"
			+ doubleToStr(currentWaveLenght.waveLenght) + "\t"
			+ doubleToStr(rCast) + "\t" + doubleToStr(rCatch) + "\t"
			+ doubleToStr(rRefl) + "\t" + doubleToStr(rI);

		log.logText(linkedLibraryOutput);

		linkedLibraryOutput = "[" + to_string(w + 1) + "/" + to_string(p->waveLenghtCount) + "]:\t" + linkedLibraryOutput;

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
