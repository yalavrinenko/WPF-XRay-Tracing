//
// Created by yalavrinenko on 02.03.18.
//
#include <Ray-tracing.hpp>
#include <iostream>


int main(int argc,char ** argv){
    RayTracing(1, "../../test/par/Order_0.par",
               [](XRayTracingLog log){
                   //std::cout << '\r' << log.linkedLibraryMinorOutput << "/" << log.linkedLibraryTotalOutput << std::endl;
               },
               [](XRayWaveResult res){
               },
               [](char const* msg, size_t count){
                    std::cout << "STDOUT:"<< std::string(msg, count) << std::endl;
               });
    return 0;
}
