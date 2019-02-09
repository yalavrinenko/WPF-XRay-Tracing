/*
 * tCylinder.cpp
 *
 *  Created on: 21.02.2014
 *      Author: cheshire
 */

#include "tCylinder.hpp"
#include <cmath>
#include <iostream>

using namespace std;

tCylinder::tCylinder() {
	r0 = Vec3d();
	R = dZ = dPhi = 0;
}

tCylinder::tCylinder(Vec3d _r0, Vec3d _RadThetaPhi, Vec3d _delta, std::string mdpName): XRTMirror(_r0, mdpName) {
	R = _RadThetaPhi.x;
	dZ = _delta.y;
	dPhi = _delta.z;
	logger.header(log_header());
}

double tCylinder::cross(tRay ray) {
	double cz = ray.b.z - r0.z;
	double cy = ray.b.y - r0.y;

	double A = ray.k.z * ray.k.z + ray.k.y * ray.k.y;
	double B = 2 * ray.k.z * cz + 2 * ray.k.y * cy;
	double C = cz * cz + cy * cy - R * R;

	double D = B * B - 4 * A * C;
	if (D < 0)
		return -1;

	double t1 = (-B + sqrt(D)) / (2 * A);
	double t2 = (-B - sqrt(D)) / (2 * A);

	double t=-1;

	if (t1>0 && checkPoint(t1, ray))
		t=t1;

	if (t2>0 && checkPoint(t2, ray)){
		if (t==-1 || dot(ray.trace(t1)) > dot(ray.trace(t2)))
			t=t2;
	}

	return t;
}

bool tCylinder::checkPoint(double t, tRay r) {
	Vec3d point = r.trace(t);

	if (fabs(point.x) > dZ)
		return false;

	double phi = asin(point.z / R);

	Vec3d hpoint = point - this->r0;

	if (hpoint.y < 0)
		return false;

	return RadToGrad(fabs(phi) ) <= dPhi;
}

tRay tCylinder::crossAndGen(tRay ray, double &t) {
	t = this->cross(ray);
	if (t <= 0) {
		t = -1;
		return {Vec3d(0, 0, 0), Vec3d(0, 0, 0), -1};
	}

	Vec3d point = ray.trace(t);

	Vec3d h_point = r0 - point;
	Vec3d ppoint = ray.k / sqrt(dot(ray.k));
	Vec3d N = h_point;
	N.x=0;
	N = N / sqrt(dot(N));

	double angle =M_PI - acos(dot2(N, ppoint) / (sqrt(dot(N)) * sqrt(dot(ppoint))));

	//--------------------------------------------------

	auto Intence = distrf(angle, ray.lambda);
	{
		rayCatch++;

		if (Intence == 2) {
			critical_guard lg(critical_section_mut);
			logger.add_data({point, Intence, ray.lambda});
			rayReflected++;
		}
	}
	//--------------------------------------------------

	Vec3d direction = ray.k / sqrt(dot(ray.k));

	Vec3d dirNormal = N * dot2(direction, N);
	Vec3d dirTangential = direction - dirNormal;

	Vec3d newDirection = dirTangential - dirNormal;

	tRay toRet(point, newDirection, ray.lambda);
	toRet.reflection_stage=Intence;
	return toRet;
}

std::string tCylinder::log_header() const {
	auto estimate_size = snprintf(nullptr, 0,
								  "#[MIRROR Cylinder]\n"
								  "#[R]\t=\t%0.10f\n"
								  "#[X,Y,Z]\t=\t[%0.10f, %0.10f, %0.10f];\n"
								  "#[Phi,Theta]\t=\t[%0.10f, %0.10f];\n"
								  "#[dPhi,dTheta]\t=\t[%0.10f, %0.10f];\n"
								  "#------------------------------------------------------------",
								  R,
								  r0.x,r0.y, r0.z,
								  dZ, dPhi,
								  dZ, dPhi);

#ifdef WIN32
	char header[1024];
#else
	char header[estimate_size + 1];
#endif
	snprintf(header, estimate_size,
			 "#[MIRROR Cylinder]\n"
			 "#[R]\t=\t%0.10f\n"
			 "#[X,Y,Z]\t=\t[%0.10f, %0.10f, %0.10f];\n"
			 "#[Phi,Theta]\t=\t[%0.10f, %0.10f];\n"
			 "#[dPhi,dTheta]\t=\t[%0.10f, %0.10f];\n"
			 "#------------------------------------------------------------",
			 R,
			 r0.x,r0.y, r0.z,
			 dZ, dPhi,
			 dZ, dPhi);

	return std::string{header};
}
