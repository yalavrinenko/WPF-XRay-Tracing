#ifndef RAY_TRACING_H
#define RAY_TRACING_H

#include <cstdint>

struct XRayTracingLog{
    int linkedLibraryMinorOutput;
    int linkedLibraryTotalOutput;
    int totalWaves;
    int currentWaves;
};

struct XRayWaveResult {
    double lambda;
    double dlambda;
    double intensity;
    double generate;
    double captured;
    double reflected;
    double relfectivity;
};

#ifdef _WIN32
typedef void(__stdcall * ProgressCallback)(XRayTracingLog);
typedef void(__stdcall * WaveCallback)(XRayWaveResult);
typedef void(__stdcall * StdOutCallback)(char const*, size_t);

#define __lib_spec __declspec(dllexport)

#else
typedef void(* ProgressCallback)(XRayTracingLog);
typedef void(* WaveCallback)(XRayWaveResult);
typedef void(* StdOutCallback)(char const*, uint64_t);
#define __lib_spec
#endif

extern "C"{
__lib_spec void terminate();
__lib_spec int RayTracing(int argc, char const* argv, ProgressCallback raysGenerated, WaveCallback waveTraced,
                          StdOutCallback stdoutCallback = nullptr);
};

#endif 
