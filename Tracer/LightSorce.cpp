/*
 * LightSorce.cpp
 *
 *  Created on: 11.02.2014
 *      Author: cheshire
 */

#include "LightSorce.hpp"

template<typename __tdistribution>
class random_generator {
public:
	random_generator(double central_line, double sigma):
		central_{ central_line }, sigma_{ sigma }, 
		is_constant_{ std::abs(sigma) < std::numeric_limits<double>::epsilon() }, distribution_{ central_, (!is_constant_) ? sigma_ : 0.1 } {
	}

	double operator() (std::mt19937_64 &engine) {
		if (!is_constant_)
			return distribution_(engine);
		else
			return central_;
	}

public:
	double central_;
	double sigma_;
	bool is_constant_;
	__tdistribution distribution_;
};

class gauss_lineshape : public random_generator<std::normal_distribution<double>> {
public:
	gauss_lineshape(double central_line, double fwhm) :
		random_generator{ central_line, fwhm / (2.0 * sqrt(2.0 * log(2.0))) } {}
};

class lourentz_lineshape : public random_generator<std::cauchy_distribution<double>> {
public:
	lourentz_lineshape(double central_line, double fwhm) :
		random_generator{ central_line, fwhm / 2.0 } {}
};

std::vector<tRay> SphereLight::GenerateRays(double lambda, double dlambda, int count) {
	gauss_lineshape distr_wavelenght(lambda, dlambda);
	//lourentz_lineshape distr_wavelenght(lambda, dlambda);

    auto single_ray = [&distr_wavelenght, this](){
        auto trg_point = target->surface_point();
        auto src_point = this->source_point();

        auto direction = trg_point - src_point;
        direction = direction / std::sqrt(dot(direction));
        tRay ray(src_point, direction, distr_wavelenght(engine()));
        ray.reflection_stage = 1;
        return ray;
    };

    std::vector<tRay> rays(static_cast<unsigned long>(count));
    for (auto &ray: rays)
        ray = single_ray();

    return std::move(rays);
}

Vec3d SphereLight::source_point() {
    auto point_theta = distr_theta(engine());
    auto point_phi = distr_phi(engine());
    auto x = std::sin(point_theta) * std::cos(point_phi) * width;
    auto y = std::sin(point_theta) * std::sin(point_phi) * height;
    auto z = std::cos(point_theta) * height;

    return Vec3d(x + r0.x, y + r0.y, z + r0.z);
}
