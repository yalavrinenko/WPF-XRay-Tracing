#include "tPlane.hpp"
#include "DetectorPlane.hpp"
#include <random>

class tObjectPlane:public tPlane{
private:
    double width;
    double height;

    TransitivityMap map;

    std::mt19937 rand_engine;
    std::uniform_real_distribution<double> uniform_rand;

public:
    tObjectPlane(Vec3d _N, Vec3d _r0, Vec2d size, TransitivityMap &t_map):
            tPlane(_N,_r0)
            ,width(size.x)
            ,height(size.y)
            ,map(std::move(t_map)){
        rand_engine = std::mt19937(std::random_device()());
        uniform_rand = std::uniform_real_distribution<double>(0.0, 1.0);
    }

    virtual tRay crossAndGen(tRay ray,double &t) override;

};
