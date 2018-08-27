/*
 * tSphere.hpp
 *
 *  Created on: 16.02.2014
 *      Author: cheshire
 */

#ifndef TSPHERE_HPP_
#define TSPHERE_HPP_

#include "tRay.hpp"
#include <vector>
#include <string>
#include "mainParameters.hpp"
#include "Object.hpp"

class tSphere: public XRTObject, public XRTMirror {
private:

	struct outVec{
		Vec3d point;
		double I,l;
	};

	Vec3d RadThetaPhi;
	Vec3d deltaRadThetaPhi;
	//x - Radius
	//y - Theta
	//z - Phi

	long long int rayCatch;
	long long int rayReflected;

	bool checkPosition(Vec3d point);
	std::string mirrorDumpFileName;

	std::vector<outVec> outArr;

	static const int rayLimit=1000;
	int rayIter;

public:
	Vec3d r0;
	FILE *fout;
	void createDumpFile(char* fileName);
	void addDumpData(Vec3d crossPoint,double I,double l);
	void setMirrorDumpFileName(std::string const &mdfName){
		mirrorDumpFileName = mdfName;
	}

public:
	tSphere() :
			r0(), RadThetaPhi() {
		mirrorDumpFileName = "";

		if (!mirrorDumpFileName.empty())
			createDumpFile((char*)mirrorDumpFileName.c_str());
	}
	tSphere(Vec3d _r0, Vec3d _RadThetaPhi, Vec3d _delta, std::string const &mdfName) :
			r0(_r0), RadThetaPhi(_RadThetaPhi), deltaRadThetaPhi(_delta), mirrorDumpFileName(mdfName) {

		if (!mirrorDumpFileName.empty())
			createDumpFile((char*)mirrorDumpFileName.c_str());
	}

	void initRayCounter() override ;
	long long int getReflRayCount() override ;
	long long int getCatchRayCount() override ;
	Vec3d GetR0() override {
		return r0;
	}

	double cross(tRay ray) override ;
	tRay crossAndGen(tRay ray,double &t) override ;

	~tSphere();

};

#endif /* TSPHERE_HPP_ */
