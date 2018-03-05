//
// Created by yalavrinenko on 02.03.18.
//
#include <Ray-tracing.hpp>
#include <iostream>


int main(int argc,char ** argv){
    RayTracing(1, "../../test/Order_1.par",
               [](XRayTracingLog log){},
               [](XRayWaveResult res){},
               [](char const* msg){
                   std::cout << "TEST " << msg << std::endl;
               });
    return 0;
}
