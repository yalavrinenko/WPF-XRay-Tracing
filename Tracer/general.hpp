/*
 * general.hpp
 *
 *  Created on: 08.02.2014
 *      Author: cheshire
 */

#ifndef GENERAL_HPP_
#define GENERAL_HPP_

#define PLANE 0
#define SPHERE 1
#define CYLINDER 2
#define GRID 3
#define DUMP_PLANE 4

#include <cmath>
#include <limits>

#ifdef __USE_OWN_PI__
constexpr double M_PI = 3.14159265358979323846264338327950288;
#endif

constexpr double VERY_BIG_NUM = std::numeric_limits<double>::max();
constexpr int VERY_BIG_NUM_INT = std::numeric_limits<int>::max();
#define RadToGrad(A) (A*180.0/M_PI)
#define GradToRad(A) (A*M_PI/180)
const double c_eps = 1E-6;

struct waveInput{
	double waveLenght;
	double dwaveLenght;
	double intensity;
};

struct Vec3d {
	double x;
	double y;
	double z;

	Vec3d(double _x, double _y, double _z) :
			x(_x), y(_y), z(_z) {
	}
	Vec3d() :
			x(0), y(0), z(0) {
	}
};

struct Vec2d {
	double x;
	double y;
	Vec2d(double _x, double _y) :
			x(_x), y(_y) {
	}
	Vec2d() :
			x(0), y(0) {
	}
};

void readReflectionFunction();

Vec3d operator-(const Vec3d &a, const Vec3d &b);

Vec3d operator+(const Vec3d &a, const Vec3d &b);

Vec3d operator*(const Vec3d &a, double t);

Vec3d operator/(const Vec3d &a, double t);

double dot(const Vec3d &a);

double dot2(const Vec3d &a, const Vec3d &b);

double normal(double mean,double stddev);

double ngrand(int a);

double rdrand(int a);

#endif /* GENERAL_HPP_ */
