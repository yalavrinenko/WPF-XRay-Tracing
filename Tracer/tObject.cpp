#include "tObject.hpp"

tRay tObjectPlane::crossAndGen(tRay ray,double &t)  {
    t = this->cross(ray);
    if (t <= 0) {
        return tRay{Vec3d{0, 0, 0}, Vec3d{0, 0, 0}, -1};
    }

    Vec3d point = ray.trace(t);

    Vec3d cross_point = this->r0 - point;

    Vec3d zDirection(0, 1, 0);
    double phi = acos(
            dot2(zDirection, N) / (sqrt(dot(zDirection)) * sqrt(dot(N))));

    double px=cross_point.x;
    double py=cross_point.y;

    cross_point.x=px*cos(phi) - py*sin(phi);
    cross_point.y=px*sin(phi) - py*cos(phi);

    if (std::fabs(cross_point.x) >= this->width/2.0 || std::fabs(cross_point.z) >= this->height / 2.0){
        return tRay{Vec3d{0, 0, 0}, Vec3d{0, 0, 0}, -1};
    }

    double TValue = map.transitivity(cross_point.x, cross_point.z);

    if (TValue < uniform_rand(rand_engine)){
        return tRay{Vec3d{0, 0, 0}, Vec3d{0, 0, 0}, -1};
    }

    ray.b = point;

    return ray;
}