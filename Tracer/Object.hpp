//
// Created by yalavrinenko on 05.03.18.
//

#ifndef XRAY_TRACING_LIB_OBJECT_HPP
#define XRAY_TRACING_LIB_OBJECT_HPP

#include "tRay.hpp"
#include "mainParameters.hpp"
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

    void setDistrFunction(double (*_distrf)(double Theta,double lambda)){
        p_distrf=_distrf;
        parameters = nullptr;
    }

    void setDistrFunction(tParameters *p){
        p_distrf=nullptr;
        parameters = p;
    }

    double distrf(double Theta, double lambda) {
        if (parameters != nullptr)
            return parameters->distr(Theta, lambda);
        if (p_distrf != nullptr)
            return p_distrf(Theta, lambda);
    }

    void setWorkingWave(double lambda){
        parameters->set_working_wave(lambda);
    }
protected:
    double (*p_distrf)(double Theta,double lambda);
    tParameters *parameters = nullptr;
};

using XRTObjectVector = std::vector<std::shared_ptr<XRTObject>>;

#endif //XRAY_TRACING_LIB_OBJECT_HPP
