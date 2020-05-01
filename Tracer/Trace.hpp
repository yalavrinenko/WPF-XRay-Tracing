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

class XRTEnvironment {
public:

  XRTEnvironment() : XRTEnvironment((unsigned) omp_get_num_procs() * 2) {
  }

  explicit XRTEnvironment(unsigned threads) :
      threads_count(threads) {
  }

  void trace(std::vector<tRay> &ray_ptr, XRTObjectVector const &xrt_objects);

  template<typename ForwardIterator, typename process_function>
  void generate_rays(ForwardIterator begin, ForwardIterator end, process_function ray_generator, std::size_t batch_size = 0);

  [[nodiscard]] size_t threads() const { return threads_count; }

protected:

  std::pair<long, long> xrt_trace_range();

private:
  unsigned threads_count;

  std::mutex access_mutex;

  const size_t XRT_DEF_BATCH_SIZE = 10000;

  struct XRTData {
    tRay *xrt_rays = nullptr;
    XRTObjectVector const *xrt_objects = nullptr;
    size_t xrt_index_begin = 0;
    size_t xrt_ray_count = 0;
    size_t xrt_batch_size = 10000;
  };

  XRTData src;
};

template<typename ForwardIterator, typename process_function>
void XRTEnvironment::generate_rays(ForwardIterator begin, ForwardIterator end, process_function ray_generator,
                                   std::size_t batch_size) {
  batch_size = (batch_size) ? batch_size : XRT_DEF_BATCH_SIZE;

  auto start = begin;
  auto next_range = [this, &start, end, batch_size](){
    std::lock_guard<std::mutex> lock(this->access_mutex);
    auto start_range = start;
    std::advance(start, std::min(static_cast<decltype(std::distance(start, end))>(batch_size), std::distance(start, end)));
    auto end_range = start;
    return std::make_pair(start_range, end_range);
  };

  auto working_function = [next_range, ray_generator]() {
    decltype(next_range()) range;
    do {
      range = next_range();
      ray_generator(range.first, range.second);
    }
    while (range.first != range.second);
  };

  std::vector<std::future<void>> thread_pool; thread_pool.reserve(threads());
  std::generate_n(std::back_inserter(thread_pool), threads(), [working_function](){
    return std::async(std::launch::async, working_function);
  });

  std::for_each(thread_pool.begin(), thread_pool.end(), [](auto &thread){
    thread.get();
  });
}

#endif /* TRACE_HPP_ */
