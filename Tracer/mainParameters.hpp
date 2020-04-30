/*
 * mainParameters.hpp
 *
 *  Created on: 14.04.2014
 *      Author: cheshire
 */

#ifndef MAINPARAMETERS_HPP_
#define MAINPARAMETERS_HPP_

#include <fstream>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <sys/stat.h>
#include <omp.h>
#include <string.h>
#include <string>
#include <vector>
#include "general.hpp"
#include "xrt_rocking_curve.h"
#include <vector>
#include <random>
#include <chrono>

using namespace std;

#define MIRROR_SPHERE  0
#define MIRROR_CYLINDER 1

class infoOut;

class tParameters {

private:
    void readWaveLenghts();

    void readReflectionFunction();

    using xrt_curve = std::pair<double, XRTRockingCurve>;

    std::vector<xrt_curve> m_reflection_curves;
    size_t m_working_curve = 0;

    std::mt19937_64 random_engine;
    std::uniform_real_distribution<double> reflection_distribution;

public:
    //GENERAL
    int rayCount = 1000000; //par
    bool isRad = true;
    int rayByIter = 1e+6;

    //SRC
    double sourceDistance{}; //par
    double sourceSize{};//par
    double sourceSize_W{};
    double sourceSize_H{};
    double aperture{}; //par
    int src_type{}; //SphereLight::cylindricType; //par
    double H{}; //par
    int orientation{}; //par

    int waveLenghtCount{};
    vector<waveInput> waveLenghts;

    //MIRROR
    double mirrorR{};//par
    double mirrorTheta = 90; //par
    double mirrorPsi = 90; //par
    double dmTh{}, dmPsi{}; //p
    double breggAngle{}; //60 //par
    double programAngle{};
    double dprogramAngle{};//0.0175; //par
    string mirrorDumpFileName;
    double refValue{};
    vector<double> reflectionFunction;
    string reflectionFileName;
    double reflStep{};
    int reflSize{};
    double crystal2d{};
    int mirrorType{};

    //OUTPUT
    string dumpPlaneName; //par
    double startPoint = 0;//43.86;
    double planeCount = 0;
    double planeStep = 0;

    double planeSize = 0;
    double planeSizeW = 0;
    double planeSizeH = 0;

    string logFileName;
    string dumpDirection;

    //OBJECT_PLANE
    double objStartPoint{};
    double objPlaneCount{};
    double objPlaneStep{};
    double objPlaneSize{};
    double objPlaneSizeH{};
    double objPlaneSizeW{};

    //OBJECT
    double gridPos{};
    double gridSize{};
    string gridType;
    double whiteConst{};
    double blackConst{};

    enum class GridLocation {
        BEFORE, AFTER
    };

    GridLocation gridLocation = GridLocation::BEFORE;
    /*double gridWidth{};
    double gridHeight{};*/
    double gridPixelSizeX{};
    double gridPixelSizeY{};
    TransitivityMap gridMap;

    tParameters() : random_engine(
            static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count())),
                    reflection_distribution(0.0, 1.0) {
    }

    explicit tParameters(char const *initFileName) : random_engine(
            static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count())),
                                            reflection_distribution(0.0, 1.0) {
        this->init(initFileName);
    }

    void init(char const *initFileName);

    double reflection(double phi, double lambda);

    void logVariable(infoOut &logger);

    void set_working_wave(double wave){
        if (m_reflection_curves.empty())
            throw std::logic_error("The curve set is empty.");

		auto nearest = std::min_element(m_reflection_curves.begin(), m_reflection_curves.end(), [wave](auto const &curve_a, auto const &curve_b) {
			return std::abs(curve_a.first - wave) < std::abs(curve_b.first - wave);
		});
		
		m_working_curve = std::distance(m_reflection_curves.begin(), nearest);
    }

    TransitivityMap const& transitivity_map() const {
        return gridMap;
    }
};

bool TestDir(string dir);

#endif /* MAINPARAMETERS_HPP_ */
