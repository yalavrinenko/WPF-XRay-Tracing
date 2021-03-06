/*
 * general.cpp
 *
 *  Created on: 16.02.2014
 *      Author: cheshire
 */

#include "general.hpp"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include "mainParameters.hpp"
using namespace std;

Vec3d operator-(const Vec3d &a, const Vec3d &b) {
	return Vec3d(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3d operator+(const Vec3d &a, const Vec3d &b) {
	return Vec3d(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3d operator*(const Vec3d &a, double t) {
	return Vec3d(a.x * t, a.y * t, a.z * t);
}

double dot(const Vec3d &a){
	return a.x*a.x+a.y*a.y+a.z*a.z;
}

Vec3d operator/(const Vec3d &a, double t) {
	return Vec3d(a.x / t, a.y / t, a.z / t);
}

double dot2(const Vec3d &a, const Vec3d &b){
	return a.x*b.x+a.y*b.y+a.z*b.z;
}

double rdrand(int a){
#ifdef WIN32
	throw "Not implemented for win32 arch";
#else
	long long int r;
	asm("rdrand %0" : "=r" (r));
	throw std::runtime_error("Use wrong rand");
	return (double)(r%a)/a;
#endif
}

double ngrand(int a){
	double r=(double)(rand()%a)/(double)a;
	double rvalue = 1.0 - 2.0*r;
	return rvalue;
}

double normal(double mean, double stddev){
	double S=0;
	double NRAND=20;
	for (int i=0; i<NRAND; i++)
		S+=ngrand(RAND_MAX);

	double normalStdDiv=2.0/sqrt(12.0);
	double value=S/(normalStdDiv*sqrt(NRAND));
	return value*stddev;
}
