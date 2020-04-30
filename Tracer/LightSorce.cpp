/*
 * LightSorce.cpp
 *
 *  Created on: 11.02.2014
 *      Author: cheshire
 */

#include "LightSorce.hpp"
#include <algorithm>

Vec3d SphereLight::random_surface_point() {
  return random_surface_point(engine());
}

Vec3d SphereLight::random_surface_point(std::mt19937_64 &random_engine) {
  auto point_theta = distr_theta(random_engine);
  auto point_phi = distr_phi(random_engine);
  auto x = std::sin(point_theta) * std::cos(point_phi) * width;
  auto y = std::sin(point_theta) * std::sin(point_phi) * height;
  auto z = std::cos(point_theta) * height;

  return Vec3d(x + r0.x, y + r0.y, z + r0.z);
}

tRay SphereLight::generate_single(double lambda, mt19937_64 &local_engine) {
  auto trg_point = target_surface_->random_surface_point(local_engine);
  auto src_point = this->random_surface_point(local_engine);

  auto direction = trg_point - src_point;
  direction = direction / std::sqrt(dot(direction));
  tRay ray(src_point, direction, lambda);
  ray.reflection_stage = 1;
  return ray;
}
