//
// Created by yalavrinenko on 05.03.18.
//

#ifndef XRAY_TRACING_LIB_OBJECT_HPP
#define XRAY_TRACING_LIB_OBJECT_HPP

#include "tRay.hpp"
#include <memory>

class XRTObject{
public:
    virtual double cross(tRay ray) = 0;

    virtual tRay crossAndGen(tRay ray, double &t) = 0;

    virtual Vec3d crossPoint(tRay) {
        throw std::logic_error(std::string("crossPoint not implemented in ")+ std::string(typeid(this).name()));
    };

    int type;
};


class XRTMirror{
public:
    virtual void initRayCounter() = 0;
    virtual long long int getCatchRayCount() = 0;
    virtual long long int getReflRayCount() = 0;
    virtual Vec3d GetR0() = 0;
};

using XRTObjectVector = std::vector<std::shared_ptr<XRTObject>>;

#endif //XRAY_TRACING_LIB_OBJECT_HPP
