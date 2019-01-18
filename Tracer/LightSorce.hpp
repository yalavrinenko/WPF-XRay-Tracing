/*
 * LightSorce.hpp
 *
 *  Created on: 09.02.2014
 *      Author: cheshire
 */

#ifndef LIGHTSORCE_HPP_
#define LIGHTSORCE_HPP_

#include "tRay.hpp"
#include "general.hpp"
#include <ctime>
#include "Object.hpp"
#include <utility>
#include <random>

class SphereLight: public XRTRaySource{
public:
	SphereLight() = default;

	SphereLight(Vec3d _position, double _Rw,double _Rh, std::shared_ptr<XRTTargetSurface> &&ray_target):
		XRTRaySource(_position, std::forward<std::shared_ptr<XRTTargetSurface>>(ray_target)),
		width(_Rw), height(_Rh),
		distr_theta(0, 180),
		distr_phi(0, 360){
	}

	std::vector<tRay> GenerateRays(double lambda, double dlambda, int count) override;

private:
	Vec3d source_point();

	double width = 0;
	double height = 0;

	std::uniform_real_distribution<double> distr_theta;
	std::uniform_real_distribution<double> distr_phi;

};

#endif /* LIGHTSORCE_HPP_ */
