//
// Created by yalavrinenko on 05.03.18.
//

#ifndef XRAY_TRACING_LIB_OBJECT_HPP
#define XRAY_TRACING_LIB_OBJECT_HPP

#include "tRay.hpp"
#include "mainParameters.hpp"
#include <memory>
#include "Utils.hpp"

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
    XRTMirror() = default;

    explicit XRTMirror(Vec3d const &center_position, std::string const &log_filename={}):
        r0(center_position), logger(log_filename){
    }

    virtual void initRayCounter() = 0;
    virtual long long int getCatchRayCount() = 0;
    virtual long long int getReflRayCount() = 0;

    virtual Vec3d const& GetR0() const{
        return r0;
    }

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
            return parameters->reflection(Theta, lambda);
        if (p_distrf != nullptr)
            return p_distrf(Theta, lambda);
    }

    void setWorkingWave(double lambda){
        parameters->set_working_wave(lambda);
    }

    Vec3d r0{};

    struct xrt_mirror_intersection{
        Vec3d point;
        double I{}, l{};

        std::string to_string() const{
            auto estimate_size = std::snprintf(nullptr, 0, "%0.10f\t%0.10f\t%0.10f\t%0.10f\t%0.10f",
                                               point.x, point.y, point.z, l, I);
            char tmp[estimate_size+1];
            std::snprintf(tmp, (size_t)estimate_size, "%0.10f\t%0.10f\t%0.10f\t%0.10f\t%0.10f",
                          point.x, point.y, point.z, l, I);

            return std::string(tmp);
        }
    };

    virtual ~XRTMirror() = default;
protected:

    virtual std::string log_header() const {
        return "No header log.";
    }

    double (*p_distrf)(double Theta,double lambda) = nullptr;
    tParameters *parameters = nullptr;
    XRTFLogging<xrt_mirror_intersection> logger;
};

using XRTObjectVector = std::vector<std::shared_ptr<XRTObject>>;

#endif //XRAY_TRACING_LIB_OBJECT_HPP
