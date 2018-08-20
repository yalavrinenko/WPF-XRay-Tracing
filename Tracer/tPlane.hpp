/*
 * tPlane.hpp
 *
 *  Created on: 08.02.2014
 *      Author: cheshire
 */

#ifndef TPLANE_HPP_
#define TPLANE_HPP_

#include "general.hpp"
#include "tRay.hpp"
#include <fstream>
#include <vector>
#include "Object.hpp"

class tPlane: public XRTObject{

public:
	Vec3d r0;
	Vec3d N;

	double limit_area;

public:
	tPlane();
	tPlane(Vec3d _N,Vec3d _r0);
	tPlane(Vec3d _N,Vec3d _r0,double _lim_area);

	virtual double cross(tRay ray) override;

	virtual tRay crossAndGen(tRay ray,double &t) override;

    virtual Vec3d crossPoint(tRay) override;
};

class tDumpPlane: public XRTObject{

	struct outVec{
		Vec3d point;
		double I;
		double lambda;
	};

public:
	Vec3d r0;
	Vec3d N;

	FILE* out;
	double limit_area;
	double limit_area_w;
	double limit_area_h;

	std::vector<outVec> tmpVector;
	int tmpVectorPosition;
	static const int tmpVectorSize=1000;

	static const int OBJECT = 1;
	static const int IMAGE = 2;

	int dumpPattern;

	void dumpHead();

public:
	tDumpPlane();
	tDumpPlane(Vec3d _N,Vec3d _r0);
	tDumpPlane(Vec3d _N,Vec3d _r0,double _lim_area,char* dumpName);
	tDumpPlane(Vec3d _N,Vec3d _r0,double _lim_area_w,double _lim_area_h,char* dumpName);

	double cross(tRay ray) override ;

	void setCrossPattern(int pattern);

	tRay crossAndGen(tRay ray,double &t) override ;

	bool check(Vec3d p);

	Vec3d crossPoint(tRay) override ;

	~tDumpPlane();
};

double defaultFunction(Vec3d p,tPlane pl);
double regularMesh(Vec3d p, tPlane pl);
double collimator(Vec3d p, tPlane pl);
double circleMesh(Vec3d p, tPlane pl);

class tGrid: public  XRTObject{
private:
	double (*transp)(Vec3d,tPlane);
	Vec3d r0;

public:
	tPlane plane;
	tGrid();
	tGrid(Vec3d _N,Vec3d _r0,double _lim_area,double (*_transp)(Vec3d,tPlane));

	tRay crossAndGen(tRay ray, double &t) override;
	Vec3d crossPoint(tRay) override {
		throw "Not implemented yet!";
	}

	double cross(tRay ray) override {
		throw "Not implemented yet!";
    }
};

#endif /* TPLANE_HPP_ */
