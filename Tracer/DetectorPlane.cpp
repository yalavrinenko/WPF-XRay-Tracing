//
// Created by yalavrinenko on 16.01.19.
//

#include "DetectorPlane.hpp"

std::string tDetectorPlane::log_header() {
    Vec3d refDirection(-1,0,0);
    double angle=acos(dot2(refDirection,this->N));

    angle*=180.0/M_PI;

    auto estimate_size = snprintf(nullptr, 0,
                                  "#[PLANE]\n"
                                  "#[ScatteringAngle]\t=\t%0.10lf\n"
                                  "#[X,Y,Z]\t=\t[%0.10lf, %0.10lf, %0.10lf];\n"
                                  "#[Nx,Ny,Nz]\t=\t[%0.10lf, %0.10lf, %0.10lf];\n"
                                  "#Size:\t=\t%0.10lf\t%0.10lf;\n"
                                  "#------------------------------------------------------------\n",
                                  angle,
                                  r0.x, r0.y, r0.z,
                                  N.x, N.y, N.z,
                                  limit_area_w,limit_area_h);

#ifdef WIN32
	char header[1024];
#else
	char header[estimate_size + 1];
#endif
    snprintf(header, (size_t) estimate_size,
             "#[PLANE]\n"
             "#[ScatteringAngle]\t=\t%0.10f\n"
             "#[X,Y,Z]\t=\t[%0.10lf, %0.10lf, %0.10lf];\n"
             "#[Nx,Ny,Nz]\t=\t[%0.10lf, %0.10lf, %0.10lf];\n"
             "#Size:\t=\t%0.10lf\t%0.10lf;\n"
             "#------------------------------------------------------------\n",
             angle,
             r0.x, r0.y, r0.z,
             N.x, N.y, N.z,
             limit_area_w,limit_area_h);

    return std::string{header};
}

double tDetectorPlane::cross(tRay ray) {
    if (ray.reflection_stage != (int)this->intersection_filter){
        return -1;
    }
    return tPlane::cross(ray);
}

