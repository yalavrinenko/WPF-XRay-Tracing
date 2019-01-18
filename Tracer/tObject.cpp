#include "tObject.hpp"

tPlane::IntersectionResult tObjectPlane::transition_decision(Vec3d const &intersection_point) {
    Vec3d cross_point = this->r0 - intersection_point;

    Vec3d zDirection(0, 1, 0);
    double phi = acos(
            dot2(zDirection, N) / (sqrt(dot(zDirection)) * sqrt(dot(N))));

    double px=cross_point.x;
    double py=cross_point.y;

    cross_point.x=px*cos(phi) - py*sin(phi);

    auto transparent = this->parameters->transitivity_map().transitivity(cross_point.x, cross_point.z);
    if (transparent < dice(r_engine))
        return IntersectionResult ::ABSORPTION;
    else
        return IntersectionResult ::TRANSMISSION;
}
