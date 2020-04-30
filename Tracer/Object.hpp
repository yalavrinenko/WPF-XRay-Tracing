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
#include "line_profile.hpp"

class XRTObject {
public:
  XRTObject() = default;

  explicit XRTObject(Vec3d const &center_position, std::string const &logpath = {}) :
      r0(center_position), logger(logpath) {
  }

  //Function to determine intersection time. Call to find nearest object.
  virtual double cross(const tRay &ray) = 0;

  //Function to make intersection at time t. Call to make intersection and get reflected ray.
  virtual tRay crossAndGen(const tRay &ray, double &t) = 0;

  virtual Vec3d const &GetR0() const {
    return r0;
  }

  void xrt_parameters(std::shared_ptr<tParameters> parameters_ptr) {
    parameters = std::move(parameters_ptr);
  }

  int type = -1;

  struct xrt_intersection {
    Vec3d point;
    double I{}, l{};

    std::string to_string() const {
      auto estimate_size = std::snprintf(nullptr, 0, "%0.10f\t%0.10f\t%0.10f\t%0.10f\t%0.10f",
                                         point.x, point.y, point.z, l, I);
#ifdef WIN32
      char tmp[1024];
#else
      char tmp[estimate_size + 1];
#endif
      std::snprintf(tmp, (size_t) estimate_size, "%0.10f\t%0.10f\t%0.10f\t%0.10f\t%0.10f",
                    point.x, point.y, point.z, l, I);

      return std::string(tmp);
    }
  };

protected:
  using critical_guard = std::lock_guard<std::mutex>;

  Vec3d r0{};

  XRTFLogging<xrt_intersection> logger;
  std::shared_ptr<tParameters> parameters = nullptr;
  std::mutex critical_section_mut;
};

template<typename random_engine_type = std::mt19937_64>
class XRTTargetSurface {
public:
  XRTTargetSurface() = default;

  explicit XRTTargetSurface(size_t seed) : engine_{seed} {
  }

  virtual Vec3d random_surface_point() = 0;

  virtual Vec3d random_surface_point(random_engine_type &random_engine) = 0;

protected:
  inline auto &engine() {
    return engine_;
  }

private:
  random_engine_type engine_{std::random_device{}()};
};

class XRTRaySource : public XRTObject, protected XRTTargetSurface<> {
public:
  XRTRaySource() = default;

  explicit XRTRaySource(Vec3d const &position, size_t seed, std::shared_ptr<XRTTargetSurface<>> target_surface) :
      XRTObject(position), XRTTargetSurface<>(seed), target_surface_{std::move(target_surface)} {
  }

  template<typename ForwardIterator>
  void generate_rays(ForwardIterator begin, ForwardIterator end, double lambda, double dlambda) {
    gauss_lineshape distr_wavelenght(lambda, dlambda);
    std::mt19937_64 local_engine{std::random_device{}()};

    auto generator = [this, &distr_wavelenght, &local_engine]() {
      return generate_single(distr_wavelenght(local_engine), local_engine);
    };
    std::generate(begin, end, generator);
  }

  double cross(const tRay &ray) final {
    throw std::logic_error(std::string("Not intersected object") + std::string(typeid(this).name()));
  }

  tRay crossAndGen(const tRay &ray, double &t) final {
    throw std::logic_error(std::string("Not intersected object") + std::string(typeid(this).name()));
  }

  static inline unsigned long r_engine_seed() {
#ifdef DEBUG_MODE
    return 42;
#else
    return std::random_device()();
#endif
  }

protected:
  virtual tRay generate_single(double lambda, std::mt19937_64 &engine) = 0;

  std::shared_ptr<XRTTargetSurface<>> target_surface_;
};

class XRTMirror : public XRTObject, public XRTTargetSurface<> {
public:
  XRTMirror() = default;

  explicit XRTMirror(Vec3d const &center_position, std::string const &log_filename = {}) :
      XRTObject(center_position, log_filename) {
  }

  virtual void initRayCounter() {
    rayCatch = rayReflected = 0;
  }

  virtual long long int getCatchRayCount() {
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

  void setWorkingWave(double lambda) {
    parameters->set_working_wave(lambda);
  }

  virtual ~XRTMirror() = default;

protected:

  std::atomic<long long int> rayCatch{};
  std::atomic<long long int> rayReflected{};
};

using XRTObjectVector = std::vector<std::shared_ptr<XRTObject>>;

#endif //XRAY_TRACING_LIB_OBJECT_HPP
