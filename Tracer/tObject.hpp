#include "tPlane.hpp"
#include "DetectorPlane.hpp"
#include <random>

class tObjectPlane:public tPlane{

public:
    tObjectPlane(Vec3d const &_N, Vec3d const &_r0, tParameters *parameters) :
            tPlane(_N, _r0, parameters->transitivity_map().size().first / 2.0,
                   parameters->transitivity_map().size().second / 2.0),
            r_engine(XRTRaySource::r_engine_seed()) {
        this->xrt_parameters(parameters);
    }

    IntersectionResult transition_decision(Vec3d const &intersection_point) override;

private:
    std::mt19937_64 r_engine;
    std::uniform_real_distribution<double> dice{0.0, 1.0};
};
