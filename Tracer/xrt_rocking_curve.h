//
// Created by yalavrinenko on 27.08.18.
//

#ifndef XRAY_TRACING_LIB_XRT_ROCKING_CURVE_H
#define XRAY_TRACING_LIB_XRT_ROCKING_CURVE_H

#include <vector>
#include <algorithm>
#include <tuple>

struct XRTCurvePoint{
    double angle = 0;
    double reflectivity = 0;
};

bool operator < (XRTCurvePoint const &, XRTCurvePoint const &b);

class XRTRockingCurve{
public:
    XRTRockingCurve() = default;

    XRTRockingCurve(std::vector<XRTCurvePoint> &data, double wave){
        bind(data, wave);
    }

    void bind(std::vector<XRTCurvePoint> &data, double wave){
        if (data.size() < 2){
            throw std::logic_error("Lack of point in rocking curve.");
        }

        m_curve = std::vector<XRTCurvePoint>(std::move(data));
        m_wave = wave;
        init();
    }

    double reflectivity(double dangle) const;

private:
    void init(){
        auto minmax = std::minmax_element(m_curve.begin(), m_curve.end());

        std::tie(min_angle, max_angle) = std::tie(minmax.first->angle, minmax.second->angle);
        angle_step = m_curve[1].angle - m_curve[0].angle;
    }

    std::vector<XRTCurvePoint> m_curve;
    double m_wave = 0;

    double min_angle = 0, max_angle = 0, angle_step = 1.0;
};

#endif //XRAY_TRACING_LIB_XRT_ROCKING_CURVE_H
