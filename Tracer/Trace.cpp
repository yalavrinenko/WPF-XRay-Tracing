/*
 * Trace.cpp
 *
 *  Created on: 16.02.2014
 *      Author: cheshire
 */

#include "Trace.hpp"
#include "general.hpp"
#include "InputOutput.hpp"

#include <vector>
#include <cmath>
#include <thread>

using namespace std;
bool writeRoad = false;

void trace_single_ray(tRay &current_ray, XRTObjectVector const &object){

    while (current_ray.lambda > 0 && current_ray.reflection_stage >= c_eps){
        double close_intersection_time = VERY_BIG_NUM;
        int intersection_object_id = -1;

        for (int j = 0; j < object.size(); j++) {
            auto intersection_time = object[j]->cross(current_ray);

            if (intersection_time > 0 && intersection_time < close_intersection_time && fabs(intersection_time) >= c_eps) {
                close_intersection_time = intersection_time;
                intersection_object_id = j;
            }
        }

        if (intersection_object_id != -1){
            auto obj = object[intersection_object_id];

            auto intersection_time = 0.0;

            auto tmp = current_ray;

            current_ray = object[intersection_object_id]->crossAndGen(current_ray, intersection_time);

        } else {
            current_ray = tRay();
        }
    }
}

std::pair<long, long> XRTEnvironment::xrt_trace_range() {
    std::lock_guard<std::mutex> lg(this->access_mutex);
    if (this->src.xrt_index_begin >= this->src.xrt_ray_count)
        return {-1, -1};

    auto begin = this->src.xrt_index_begin;
    auto end = begin + this->src.xrt_batch_size;
    if (end > this->src.xrt_ray_count)
        end = this->src.xrt_ray_count;

    this->src.xrt_index_begin = end;
    return {begin, end};
}

void XRTEnvironment::trace(tRay *ray_ptr, size_t count, XRTObjectVector const &xrt_objects) {
    src = {
            ray_ptr,
            &xrt_objects,
            0,
            count,
            XRT_DEF_BATCH_SIZE
    };

    auto pool_function = [this](){
        auto range = this->xrt_trace_range();

        auto batches = 0u;
        while (range.first != -1) {
            for (auto it = range.first; it < range.second; ++it) {
                trace_single_ray(this->src.xrt_rays[it], *(this->src.xrt_objects));
            }

            range = this->xrt_trace_range();
            ++batches;
        }

        return batches;
    };

    std::vector<std::future<unsigned>> task_futures;

    task_futures.reserve(this->threads_count);
    for (auto i = 0; i < this->threads_count; ++i) {
        task_futures.emplace_back(std::async(std::launch::async, pool_function));
    }

    std::for_each(task_futures.begin(), task_futures.end(), [](auto &future){
        future.get();
    });
}
