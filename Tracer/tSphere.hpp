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
public:
	tSphere() = default;

	tSphere(Vec3d _r0, Vec3d _RadThetaPhi, Vec3d _delta, std::string const &mdfName) :
			XRTMirror(_r0, mdfName),
			RadThetaPhi(_RadThetaPhi), deltaRadThetaPhi(_delta){
	}

	void initRayCounter() override ;
	long long int getReflRayCount() override ;
	long long int getCatchRayCount() override ;

	double cross(tRay ray) override ;
	tRay crossAndGen(tRay ray,double &t) override ;

	~tSphere() = default;

protected:
	virtual std::string log_header() const override;
private:
	Vec3d RadThetaPhi;
	Vec3d deltaRadThetaPhi;
	//x - Radius
	//y - Theta
	//z - Phi

	long long int rayCatch{};
	long long int rayReflected{};

	bool checkPosition(Vec3d point);
};

#endif /* TSPHERE_HPP_ */
