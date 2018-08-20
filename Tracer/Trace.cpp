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

using namespace std;
bool writeRoad=false;

void rayTrace(tRay* ray, int ray_count, XRTObjectVector &object,
		int obj_count) {
	bool flag = true;

	long proccessed=0;

	vector<vector<Vec3d> > road;
	if (writeRoad)
		road.resize(ray_count);

	vector<Vec3d> toRet;

	while (flag) {
		flag = false;

		for (int i = 0; i < ray_count; i++) {
			tRay r = ray[i];

			if (r.lambda <= 0 || fabs(r.I) <= c_eps)
				continue;

			if (writeRoad)
				road[i].push_back(r.b);

			double t;

			double nearestObj = VERY_BIG_NUM;
			int nearestObjId = VERY_BIG_NUM_INT;

			tRay r_out;

			for (int j = 0; j < obj_count; j++) {
				t = object[j]->cross(r);

				if (t < nearestObj && t > 0 && fabs(t) >= c_eps) {
					nearestObj = t;
					nearestObjId = j;
				}
			}

			if (nearestObj != VERY_BIG_NUM) {
				auto obj=object[nearestObjId];

				ray[i] = object[nearestObjId]->crossAndGen(r, t);

				if (ray[i].I==0)
					proccessed++;
				flag = true;
			} else {

				if (writeRoad)
					road[i].push_back(ray[i].k * 10 + ray[i].b);

				ray[i] = tRay();
				proccessed++;
			}
		}
	}

	if (writeRoad)
		dumpRoadNOSPH("Road.m", "Road", road);
}
