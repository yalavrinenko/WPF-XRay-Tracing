//
// Created by yalavrinenko on 16.01.19.
//

#ifndef XRAY_TRACING_LIB_DETECTORPLANE_HPP
#define XRAY_TRACING_LIB_DETECTORPLANE_HPP

#include "tPlane.hpp"

class tDetectorPlane: public tPlane{
public:
    enum class IntersectionFilter {
        OBJECT = 1,
        IMAGE = 2
    };

    tDetectorPlane() = default;

    tDetectorPlane(Vec3d _N,Vec3d _r0):
            tDetectorPlane(_N, _r0, 0, 0, {}){
    }

    tDetectorPlane(Vec3d _N,Vec3d _r0,double _lim_area, std::string const &dumpName):
            tDetectorPlane(_N, _r0, _lim_area, _lim_area, dumpName){
    }

    tDetectorPlane(Vec3d _N,Vec3d _r0,double _lim_area_w,double _lim_area_h,std::string const &dumpName):
        tPlane(_N, _r0, _lim_area_w, _lim_area_h, dumpName){
        logger.header(log_header());
    }

    void setCrossPattern(IntersectionFilter pattern) {
        intersection_filter = pattern;
    }

    double cross(tRay ray) override;

    tRay crossAndGen(tRay ray,double &t) override;

    ~tDetectorPlane() = default;

protected:
    std::string log_header();

private:
    IntersectionFilter intersection_filter{};
};

#endif //XRAY_TRACING_LIB_DETECTORPLANE_HPP
