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
	Vec3d N;

	double limit_area_w{};
	double limit_area_h{};

public:
	tPlane() = default;

	tPlane(Vec3d _N, Vec3d _r0, std::string const &logpath = {});

	tPlane(Vec3d _N, Vec3d _r0, double _lim_area, std::string const &logpath = {});

	tPlane(Vec3d _N,Vec3d _r0,double _lim_area_w,double _lim_area_h,std::string const &dumpName);

	virtual double cross(tRay ray) override;

	virtual tRay crossAndGen(tRay ray,double &t) override;

protected:
	virtual bool transition_decision(Vec3d const &intersection_point) {
		return true;
	}

	virtual bool intersection_check(Vec3d const &intercetion_point);

};

#endif /* TPLANE_HPP_ */
