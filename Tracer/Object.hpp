//
// Created by yalavrinenko on 05.03.18.
//

#ifndef XRAY_TRACING_LIB_OBJECT_HPP
#define XRAY_TRACING_LIB_OBJECT_HPP

#include "tRay.hpp"
#include "mainParameters.hpp"
#include <memory>
#include "Utils.hpp"
#include <mutex>
#include <atomic>

class XRTObject{
public:
    XRTObject() = default;

    explicit XRTObject(Vec3d const &center_position, std::string const &logpath = {}):
        r0(center_position), logger(logpath){
    }

    //Function to determine intersection time. Call to find nearest object.
    virtual double cross(tRay ray) = 0;

    //Function to make intersection at time t. Call to make intersection and get reflected ray.
    virtual tRay crossAndGen(tRay ray, double &t) = 0;

    virtual Vec3d const& GetR0() const{
        return r0;
    }

    void xrt_parameters(tParameters *parameters_ptr){
        parameters = parameters_ptr;
    }

    int type = -1;

    struct xrt_intersection{
        Vec3d point;
        double I{}, l{};

        std::string to_string() const{
            auto estimate_size = std::snprintf(nullptr, 0, "%0.10f\t%0.10f\t%0.10f\t%0.10f\t%0.10f",
                                               point.x, point.y, point.z, l, I);
#ifdef WIN32
			char tmp[1024];
#else
            char tmp[estimate_size+1];
#endif
            std::snprintf(tmp, (size_t)estimate_size, "%0.10f\t%0.10f\t%0.10f\t%0.10f\t%0.10f",
                          point.x, point.y, point.z, l, I);

            return std::string(tmp);
        }
    };

protected:
    using critical_guard = std::lock_guard<std::mutex>;

    Vec3d r0{};

    XRTFLogging<xrt_intersection> logger;
    tParameters *parameters = nullptr;
    std::mutex critical_section_mut;
};


class XRTMirror: public XRTObject{
public:
    XRTMirror() = default;

    explicit XRTMirror(Vec3d const &center_position, std::string const &log_filename={}):
        XRTObject(center_position, log_filename){
    }

    virtual void initRayCounter() {
        rayCatch=rayReflected=0;
    }

    virtual long long int getCatchRayCount(){
        return this->rayCatch.load();
    }

    virtual long long int getReflRayCount() {
        return this->rayReflected.load();
    }

    double distrf(double Theta, double lambda) {
        if (parameters != nullptr)
            return parameters->reflection(Theta, lambda);
        else
            throw std::logic_error("Error in parameters structure at" + std::string(__FUNCTION__));
    }

    void setWorkingWave(double lambda){
        parameters->set_working_wave(lambda);
    }

    virtual ~XRTMirror() = default;
protected:

    std::atomic<long long int> rayCatch{};
    std::atomic<long long int> rayReflected{};
};

using XRTObjectVector = std::vector<std::shared_ptr<XRTObject>>;

#endif //XRAY_TRACING_LIB_OBJECT_HPP
