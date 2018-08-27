cmake_minimum_required(VERSION 2.8)


project(xray-tracing-lib)

set(CMAKE_CXX_STANDARD 14)

set (SOURCES general.cpp
        gridFunction.cpp
        InputOutput.cpp
        LightSorce.cpp
        mainParameters.cpp
        param.cpp
        Ray-tracing.cpp
        tCylinder.cpp
        tObject.cpp
        tPlane.cpp
        Trace.cpp
        tRay.cpp
        tSphere.cpp TransitivityMap.cpp TransitivityMap.hpp)

set (HEADERS general.hpp
        InputOutput.hpp
        LightSorce.hpp
        mainParameters.hpp
        param.h
        Ray-tracing.hpp
        tCylinder.hpp
        tMirror.hpp
        tObject.hpp
        tPlane.hpp
        Trace.hpp
        tRay.hpp
        tSphere.hpp
        Object.hpp
        TransitivityMap.cpp TransitivityMap.hpp xrt_rocking_curve.cpp xrt_rocking_curve.h)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


option(TEST_MODE "Set seed for rand to 42" OFF)

if (TEST_MODE)
    add_definitions(-DDEUBG_MODE)
endif()

add_library(xray-tracing SHARED ${SOURCES} ${HEADERS})

add_subdirectory(test)