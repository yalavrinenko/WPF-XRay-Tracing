/*
 * LightSorce.cpp
 *
 *  Created on: 11.02.2014
 *      Author: cheshire
 */

#include "LightSorce.hpp"

std::vector<tRay> SphereLight::GenerateRays(double lambda, double dlambda, int count) {
    std::normal_distribution<double> distr_wavelenght(lambda, dlambda / (sqrt(2.0 * log(2.0))));

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
