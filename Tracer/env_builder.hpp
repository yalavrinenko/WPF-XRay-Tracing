//
// Created by yalavrinenko on 01.05.2020.
//

#ifndef XRAY_TRACING_LIB_ENV_BUILDER_HPP
#define XRAY_TRACING_LIB_ENV_BUILDER_HPP

#include "LightSorce.hpp"
#include "tCylinder.hpp"
#include "tObject.hpp"
#include "tSphere.hpp"

class XRTSystem{
public:
  using ptParameters = std::shared_ptr<tParameters>;

  enum class PlaneAlignment{
    RolandCircleCenter,
    CrystalNormal
  };

  explicit XRTSystem(ptParameters parameters) : parameters_{std::move(parameters)}{
    create_mirror();
    create_light_source();
    create_detector();
  }

  void create_plane(double distance, double angle, Size size,
                    tDetectorPlane::IntersectionFilter crossPattern,
                    XRTSystem::PlaneAlignment alignment, Vec3d align_base,
                    std::string const &dump_name);

  ~XRTSystem();
private:
  void create_mirror();

  void create_light_source();

  void create_detector();

  struct XRTSystemImpl;
  std::unique_ptr<XRTSystemImpl> system_;

  std::shared_ptr<tParameters> parameters_;
  XRTObjectVector objects_;
};



#endif//XRAY_TRACING_LIB_ENV_BUILDER_HPP
