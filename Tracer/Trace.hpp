/*
 * Trace.hpp
 *
 *  Created on: 16.02.2014
 *      Author: cheshire
 */

#ifndef TRACE_HPP_
#define TRACE_HPP_
#include "tRay.hpp"
#include "tPlane.hpp"
#include "tSphere.hpp"
#include "tCylinder.hpp"
#include <vector>
#include <memory>
#include <omp.h>
#include <future>

void trace_single_ray(tRay &current_ray, XRTObjectVector const &object);

class XRTEnvironment{
public:

    XRTEnvironment(): XRTEnvironment((unsigned )omp_get_num_procs() * 2){
    }

    explicit XRTEnvironment(unsigned threads):
            threads_count(threads){
    }

    void trace(tRay* ray_ptr, size_t count, XRTObjectVector const &xrt_objects);

protected:

    std::pair<long, long> xrt_trace_range();

private:
    std::vector<std::packaged_task<unsigned ()>> task_pool;
    unsigned threads_count;

    std::mutex access_mutex;

    const size_t XRT_DEF_BATCH_SIZE = 10000;

    struct XRTData{
        tRay* xrt_rays = nullptr;
        XRTObjectVector const* xrt_objects = nullptr;
        size_t xrt_index_begin = 0;
        size_t xrt_ray_count = 0;
        size_t xrt_batch_size = 10000;
    };

    XRTData src;
};

#endif /* TRACE_HPP_ */
