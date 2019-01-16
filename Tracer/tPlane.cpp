/*
 * tPlane.cpp
 *
 *  Created on: 21.02.2014
 *      Author: cheshire
 */

#include "tPlane.hpp"

#include <iostream>
#include <cmath>

using namespace std;

tPlane::tPlane(Vec3d _N, Vec3d _r0, std::string const &logpath) :
        tPlane(_N, _r0, 0, 0, logpath) {
}

tPlane::tPlane(Vec3d _N, Vec3d _r0, double _lim_area, std::string const &logpath) :
        tPlane(_N, _r0, _lim_area, _lim_area, logpath){
}

tPlane::tPlane(Vec3d _N, Vec3d _r0, double _lim_area_w, double _lim_area_h, std::string const &dumpName) :
        XRTObject(_r0, dumpName),
        N(_N / sqrt(dot(_N))),
        limit_area_h(_lim_area_h),
        limit_area_w(_lim_area_w) {
}


double tPlane::cross(tRay ray) {
    double p2 = dot2(N, ray.k);
    if (p2 < 0) {
        N.x = -N.x;
        N.y = -N.y;
        N.z = -N.z;
        p2 = dot2(N, ray.k);
    }

    double p1 = dot2(N, ray.b - r0);
    double t = -(p1 / p2);

    if (intersection_check(ray.trace(t)))
        return t;
    else
        return -1;
}

tRay tPlane::crossAndGen(tRay ray, double &t) {
    t = this->cross(ray);
    if (t <= 0) {
        t = -1;
        return {Vec3d(0, 0, 0), Vec3d(0, 0, 0), -1};
    }

    Vec3d point = ray.trace(t);

    auto is_transition = transition_decision(point);

    if (!is_transition) {
        return tRay(Vec3d(0, 0, 0), Vec3d(0, 0, 0), -1);
    }

    Vec3d direction = ray.k / sqrt(dot(ray.k));

    double P = RadToGrad(acos(dot2(direction, N) / (sqrt(dot(direction)) * sqrt(dot(N)))));

    Vec3d dirNormal = N * dot2(direction, N);
    Vec3d dirTangential = direction - dirNormal;

    Vec3d newDirection = dirTangential - dirNormal;

    P = RadToGrad(acos(dot2(newDirection, N) / (sqrt(dot(newDirection)) * sqrt(dot(N)))));

    tRay toRet(point, newDirection, ray.lambda);
    toRet.reflection_stage = ray.reflection_stage;

    {
        critical_guard lg(critical_section_mut);
        logger.add_data({point, toRet.reflection_stage, toRet.lambda});
    }

    return toRet;
}

bool tPlane::intersection_check(Vec3d const &intercetion_point) {
    Vec3d zDirection(0, 1, 0);
    double phi = acos(
            dot2(zDirection, N) / (sqrt(dot(zDirection)) * sqrt(dot(N))));

    Vec3d op = intercetion_point - this->r0;
    double px = op.x;
    double py = op.y;
    double pz = op.z;

    op.x = px * cos(phi) - py * sin(phi);
    op.y = px * sin(phi) - py * cos(phi);

    return !(fabs(op.x) > limit_area_w || fabs(op.z) > limit_area_h);
}
