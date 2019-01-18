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

class tSphere: public XRTMirror {
public:
	tSphere() = default;

	tSphere(Vec3d _r0, Vec3d _RadThetaPhi, Vec3d _delta, std::string const &mdfName) :
			XRTMirror(_r0, mdfName),
			RadThetaPhi(_RadThetaPhi), deltaRadThetaPhi(_delta),
			distr_theta(RadThetaPhi.y - deltaRadThetaPhi.y, RadThetaPhi.y + deltaRadThetaPhi.y),
			distr_phi(RadThetaPhi.z - deltaRadThetaPhi.z, RadThetaPhi.z + deltaRadThetaPhi.z){
		logger.header(log_header());
	}

	//From xray_object
	double cross(tRay ray) override ;
	tRay crossAndGen(tRay ray,double &t) override ;

	//From raytarget
	Vec3d surface_point() override;

	~tSphere() override = default;

protected:
	std::string log_header() const;

private:
	Vec3d RadThetaPhi;
	Vec3d deltaRadThetaPhi;
	//x - Radius
	//y - Theta
	//z - Phi

	std::uniform_real_distribution<double> distr_theta, distr_phi;

	bool checkPosition(Vec3d point);
};

#endif /* TSPHERE_HPP_ */
