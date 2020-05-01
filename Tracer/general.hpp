/*
 * general.hpp
 *
 *  Created on: 08.02.2014
 *      Author: cheshire
 */

#ifndef GENERAL_HPP_
#define GENERAL_HPP_

#include <cmath>
#include <limits>
#include <vector>
#include <string>
#include "TransitivityMap.hpp"

struct waveInput{
	double waveLenght;
	double dwaveLenght;
	double intensity;
};

struct Size{
  double width;
  double height;
  double depth;
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

Vec3d operator-(const Vec3d &a, const Vec3d &b);

Vec3d operator+(const Vec3d &a, const Vec3d &b);

Vec3d operator*(const Vec3d &a, double t);

Vec3d operator/(const Vec3d &a, double t);

struct MathExtension{
  inline static double pi() { static double pi_ = std::atan(1.0) * 4.0; return pi_; }
  static inline double RadToGrad(double angle) { return angle * 180.0 / MathExtension::pi(); }
  static inline double GradToRad(double angle) { return angle * MathExtension::pi() / 180.0; }
};


double dot(const Vec3d &a);

double dot2(const Vec3d &a, const Vec3d &b);

double ngrand(int a);

#endif /* GENERAL_HPP_ */
