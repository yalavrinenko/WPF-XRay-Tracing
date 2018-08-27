//
// Created by yalavrinenko on 27.08.18.
//

#include "xrt_rocking_curve.h"
#include <iostream>

bool operator < (XRTCurvePoint const &a, XRTCurvePoint const &b){
    return  a.angle < b.angle;
}

double XRTRockingCurve::reflectivity(double dangle) const {
    if (dangle < min_angle || dangle > max_angle)
        return 0.0;

    dangle +=  -min_angle;

    auto k = (size_t)(dangle / angle_step);
    auto k1 = k + 1;

    if (k1 >= m_curve.size()) k1 = k;
    auto reflectivity = m_curve[k].reflectivity +
            (m_curve[k1].reflectivity - m_curve[k].reflectivity) / angle_step * (dangle - k * angle_step);

    if (reflectivity>1 || reflectivity<0.0)
        std::cout<<"Reflection interpolation error!"<< std::endl;

	return reflectivity;
}
