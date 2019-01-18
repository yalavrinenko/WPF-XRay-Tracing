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

class tCylinder: public XRTMirror{
public:
	tCylinder();
	tCylinder(Vec3d _r0, Vec3d _RadThetaPhi, Vec3d _delta, std::string mdfName);

	double cross(tRay ray) override;
	tRay crossAndGen(tRay ray,double &t) override ;

	//From raytarget
	Vec3d surface_point() override;

	~tCylinder() override = default;

protected:
	std::string log_header() const;

private:
	double R;
	double dZ; //center At y = y0
	double dPhi;  //center At Phi = 90;

	bool checkPoint(double t, tRay r);

	std::uniform_real_distribution<double> distr_phi, distr_z;
};


#endif /* TCYLINDER_HPP_ */
