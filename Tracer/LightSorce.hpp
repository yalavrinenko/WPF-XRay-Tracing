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

	SphereLight(Vec3d position, double Rw, double Rh, size_t seed, std::shared_ptr<XRTTargetSurface<>> target_surface):
		XRTRaySource(position, seed, std::move(target_surface)),
		width(Rw), height(Rh),
		distr_theta(0, 180),
		distr_phi(0, 360){
	}

protected:
  Vec3d random_surface_point() override;

  Vec3d random_surface_point(mt19937_64 &random_engine) override;

  tRay generate_single(double lambda, mt19937_64 &local_engine) override;

private:
	double width = 0;
	double height = 0;

	std::uniform_real_distribution<double> distr_theta;
	std::uniform_real_distribution<double> distr_phi;

};

#endif /* LIGHTSORCE_HPP_ */
