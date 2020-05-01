//
// Created by yalavrinenko on 01.05.2020.
//

#ifndef XRAY_TRACING_LIB_ENV_BUILDER_HPP
#define XRAY_TRACING_LIB_ENV_BUILDER_HPP

#include "Object.hpp"
#include <memory>

class tParameters;

class XRTSystem{
public:
  using ptParameters = std::shared_ptr<tParameters>;

  enum class PlaneAlignment{
    RolandCircleCenter,
    CrystalNormal
  };

  explicit XRTSystem(ptParameters parameters);

  void create_plane(double distance, double angle, Size size, IntersectionFilter crossPattern,
                    XRTSystem::PlaneAlignment alignment, Vec3d align_base,
                    std::string const &dump_name);

  shared_ptr<XRTMirror>& crystal();
  std::unique_ptr<XRTRaySource>& source();
  XRTObjectVector & objects() { return objects_; }

  [[nodiscard]] shared_ptr<XRTMirror> const & crystal() const;
  [[nodiscard]] std::unique_ptr<XRTRaySource> const & source() const;
  XRTObjectVector const& objects() const  { return objects_; }

  ~XRTSystem();
private:
  void create_object(double distance, double angle, Size size);

  void create_mirror();

  void create_light_source();

  void create_detector();

  struct XRTSystemImpl;
  std::unique_ptr<XRTSystemImpl> system_;

  std::shared_ptr<tParameters> parameters_;
  XRTObjectVector objects_;
};



#endif//XRAY_TRACING_LIB_ENV_BUILDER_HPP
