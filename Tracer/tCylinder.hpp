/*
 * tCylinder.hpp
 *
 *  Created on: 21.02.2014
 *      Author: cheshire
 */

#ifndef TCYLINDER_HPP_
#define TCYLINDER_HPP_

#include "tRay.hpp"
#include "general.hpp"
#include "tPlane.hpp"
#include "mainParameters.hpp"

class tCylinder: public XRTObject, public XRTMirror{
private:
	struct outVec{
		Vec3d point;
		double I,l;
	};

	double R;
	double dZ; //center At y = y0
	double dPhi;  //center At Phi = 90;

	bool checkPoint(double t, tRay r);

	long long int rayCatch;
	long long int rayReflected;
	static const int rayLimit=1000;

	std::string mirrorDumpFileName;
	std::vector<outVec> outArr;
	int rayIter;

public:

	Vec3d r0;

	FILE *fout;
	void createDumpFile(char* fileName);
	void addDumpData(Vec3d crossPoint,double I,double l);

	tCylinder();
	tCylinder(Vec3d _r0, Vec3d _RadThetaPhi, Vec3d _delta, std::string mdfName);

	double cross(tRay ray) override;
	tRay crossAndGen(tRay ray,double &t) override ;

	void initRayCounter() override ;
	long long int getReflRayCount() override ;
	long long int getCatchRayCount() override ;

	Vec3d GetR0() override {
		return r0;
	}

	double setMirrorDumpFileName(std::string const &name){
		this->mirrorDumpFileName=name;
	}

	~tCylinder();
};


#endif /* TCYLINDER_HPP_ */
