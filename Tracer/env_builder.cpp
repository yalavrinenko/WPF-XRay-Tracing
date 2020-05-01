//
// Created by yalavrinenko on 01.05.2020.
//

#include "env_builder.hpp"

struct XRTSystem::XRTSystemImpl {
  std::shared_ptr<XRTMirror> mirror_;
  std::unique_ptr<XRTRaySource> source_;

  template<typename crystall_type>
  void create_mirror(ptParameters const &parameters);

  template<typename ray_source>
  void create_ray_source(ptParameters const &parameters);
};

template<typename crystal_type>
void XRTSystem::XRTSystemImpl::create_mirror(ptParameters const &parameters) {
  double mirrorYpos =
      parameters->sourceDistance * cos(parameters->programAngle) - parameters->mirrorR;
  Vec3d mpos(0, mirrorYpos, 0);
  Vec3d mprop(parameters->mirrorR, parameters->mirrorTheta, parameters->mirrorPsi);
  Vec3d mdprop(0, parameters->dmTh, parameters->dmPsi);

  mirror_ = std::make_shared<crystal_type>(mpos, mprop, mdprop, parameters->mirrorDumpFileName);
  mirror_->xrt_parameters(parameters);
}
template<typename ray_source>
void XRTSystem::XRTSystemImpl::create_ray_source(const XRTSystem::ptParameters &parameters) {
  double srcXpos = parameters->sourceDistance * sin(parameters->programAngle);
  Vec3d spos(-srcXpos, 0, 0);
  source_ = std::make_unique<ray_source>(spos, parameters->sourceSize_W, parameters->sourceSize_H,
                                         std::random_device{}(), mirror_);

}

void XRTSystem::create_mirror() {
  if (parameters_->mirrorType == MIRROR_SPHERE) system_->create_mirror<tSphere>(parameters_);
  if (parameters_->mirrorType == MIRROR_CYLINDER) system_->create_mirror<tCylinder>(parameters_);
  objects_.push_back(system_->mirror_);
}
void XRTSystem::create_light_source() {
  system_->create_ray_source<SphereLight>(parameters_);
}
void XRTSystem::create_plane(double distance, double angle, Size size,
                             tDetectorPlane::IntersectionFilter crossPattern,
                             XRTSystem::PlaneAlignment alignment, Vec3d align_base,
                             std::string const &dump_name) {
  auto sx = distance * sin(angle);
  auto sy = distance * cos(angle);
  auto base = system_->mirror_->GetR0() + Vec3d{0, parameters_->mirrorR, 0};
  sy = base.y - sy;

  Vec3d r0(sx, sy, 0);
  Vec3d N = align_base - r0;
  N = N / sqrt(dot(N));

  if (alignment == PlaneAlignment::RolandCircleCenter) {
    auto nx = N.x * cos(-MathExtension::pi() / 2.0) - N.y * sin(-MathExtension::pi() / 2.0);
    auto ny = N.x * sin(-MathExtension::pi() / 2.0) + N.y * cos(-MathExtension::pi() / 2.0);
    N.x = nx;
    N.y = ny;
  }

  string name = dump_name;

  auto dp = std::make_shared<tDetectorPlane>(N, r0, size.width, size.height, name);
  dp->setCrossPattern(crossPattern);
  dp->xrt_parameters(parameters_);

  objects_.push_back(dp);
}
void XRTSystem::create_detector() {
  create_plane(parameters_->startPoint, parameters_->programAngle,
                 {parameters_->objPlaneSizeW, parameters_->objPlaneSizeH, 0.0},
                 tDetectorPlane::IntersectionFilter::IMAGE,
                 PlaneAlignment::RolandCircleCenter, system_->mirror_->GetR0(), parameters_->dumpPlaneName + "Detector.dmp");
}

XRTSystem::~XRTSystem() = default;