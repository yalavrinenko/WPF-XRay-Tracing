/*
 * tShere.cpp
 *
 *  Created on: 16.02.2014
 *      Author: cheshire
 */

#include "tSphere.hpp"
#include "InputOutput.hpp"

using namespace std;

double tSphere::cross(const tRay &ray) {
    double A = dot(ray.k);
    double B = 2 * (ray.b.x - this->r0.x) * ray.k.x
               + 2 * (ray.b.y - this->r0.y) * ray.k.y
               + 2 * (ray.b.z - this->r0.z) * ray.k.z;
    double C = dot(ray.b - this->r0)
               - this->RadThetaPhi.x * this->RadThetaPhi.x;
    //At*t+B*t+C=0

    double D = B * B - 4 * A * C;
    if (D < 0)
        return -1;
    double t1 = (-B + sqrt(D)) / (2 * A);
    double t2 = (-B - sqrt(D)) / (2 * A);

    if (checkPosition(ray.trace(t1)))
        return t1;

    if (checkPosition(ray.trace(t2)))
        return t2;

    return -1;
}

tRay tSphere::crossAndGen(const tRay &ray, double &t) {
    double crossPoint = this->cross(ray);
    t = crossPoint;
    if (crossPoint == -1)
        return tRay(Vec3d(0, 0, 0), Vec3d(0, 0, 0), -1);

    Vec3d point = ray.trace(crossPoint);

    Vec3d N = (point - this->r0) / this->RadThetaPhi.x;

    Vec3d direction = ray.k / sqrt(dot(ray.k));

    double cosA = dot2(N, direction) / (sqrt(dot(N)) * sqrt(dot(direction)));

    if (cosA > 1)
        cosA = 1;

    double angle = acos(cosA);

    //--------------------------------------------------
    auto Intence = distrf(angle, ray.lambda);
    {
        rayCatch++;

        if (Intence == 2) {
            critical_guard lg(critical_section_mut);
            logger.add_data({point, Intence, ray.lambda});
            rayReflected++;
        }
    }
    //--------------------------------------------------

    Vec3d dirNormal = N * dot2(direction, N);
    Vec3d dirTangential = direction - dirNormal;

    Vec3d newDirection = dirTangential - dirNormal;

    tRay toRet(point, newDirection, ray.lambda);
    toRet.reflection_stage = Intence;
    return toRet;
}

bool tSphere::checkPosition(Vec3d point) {
    Vec3d R = point - this->r0;
    double r = sqrt(dot(R));
    double eps = 1e-10;
    if (r - eps >= this->RadThetaPhi.x && this->RadThetaPhi.x >= r + eps)
        return false;
    double Theta = acos(R.z / r);
    double Phi = acos(R.x / (r * sin(Theta)));

    if (std::isnan(Phi))
        Phi = M_PI;

    if (R.y < 0)
        Phi = M_PI + Phi;

    Theta = RadToGrad(Theta);
    Phi = RadToGrad(Phi);

    return fabs(RadThetaPhi.y - Theta) <= deltaRadThetaPhi.y
           && fabs(RadThetaPhi.z - Phi) <= deltaRadThetaPhi.z;

}

std::string tSphere::log_header() const {
    auto estimate_size = snprintf(nullptr, 0,
                                  "#[MIRROR Sphere]\n"
                                  "#[R]\t=\t%0.10f\n"
                                  "#[X,Y,Z]\t=\t[%0.10f, %0.10f, %0.10f];\n"
                                  "#[Phi,Theta]\t=\t[%0.10f, %0.10f];\n"
                                  "#[dPhi,dTheta]\t=\t[%0.10f, %0.10f];\n"
                                  "#------------------------------------------------------------",
                                  RadThetaPhi.x, r0.x,
                                  r0.y, r0.z,
                                  RadThetaPhi.z,
                                  RadThetaPhi.y, deltaRadThetaPhi.z, deltaRadThetaPhi.y);

    char header[estimate_size + 1];
    snprintf(header, (size_t) estimate_size,
             "#[MIRROR Sphere]\n"
             "#[R]\t=\t%0.10f\n"
             "#[X,Y,Z]\t=\t[%0.10f, %0.10f, %0.10f];\n"
             "#[Phi,Theta]\t=\t[%0.10f, %0.10f];\n"
             "#[dPhi,dTheta]\t=\t[%0.10f, %0.10f];\n"
             "#------------------------------------------------------------",
             RadThetaPhi.x,
             r0.x,
             r0.y, r0.z,
             RadThetaPhi.z,
             RadThetaPhi.y, deltaRadThetaPhi.z, deltaRadThetaPhi.y);

    return std::string{header};
}

Vec3d tSphere::surface_point() {
    auto point_theta = GradToRad(distr_theta(engine()));
    auto point_phi = GradToRad(distr_phi(engine()));
    auto &r = this->RadThetaPhi.x;
    auto x = r * std::sin(point_theta) * std::cos(point_phi);
    auto y = r * std::sin(point_theta) * std::sin(point_phi);
    auto z = r * std::cos(point_theta);

    return {x + r0.x, y + r0.y, z + r0.z};
}
