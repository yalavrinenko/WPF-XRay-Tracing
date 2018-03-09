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

void rayTrace(tRay* ray, int ray_count, XRTObjectVector &object, int obj_count);

#endif /* TRACE_HPP_ */
