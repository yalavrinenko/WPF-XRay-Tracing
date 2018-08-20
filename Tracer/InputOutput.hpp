/*
 * InputOutput.hpp
 *
 *  Created on: 17.02.2014
 *      Author: cheshire
 */

#ifndef INPUTOUTPUT_HPP_
#define INPUTOUTPUT_HPP_

#include "general.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include "tRay.hpp"
#include <string>
#include "tSphere.hpp"
#include "LightSorce.hpp"
#include "tCylinder.hpp"
#include <sstream>
#include <functional>
#include <memory>

using namespace std;

void dump(char* name,char* name_func,tRay *arr,int count,double *t);
void dumpRef(char* name,char* name_func,tRay* arr,int count,double *t);
void dumpRoad(char* name,char* name_func,vector<vector<Vec3d> > road);
void dumpRoadNOSPH(char const* name, char const* name_func, vector<vector<Vec3d> > road);
string doubleToStr(double a);
string intToStr(int a);
void dumpPlane(char* name,vector<Vec3d> odata);

void dumpRays(tRay* d,char* fileName,int count, Vec3d dir);

void appPoint(Vec3d point);
void dumpPoint(char* name);

class infoOut{
private:
	ofstream out;

    std::function<void(char const*, size_t)> _callback = nullptr;
public:
	infoOut();
	infoOut(char const* name);
    infoOut(char const* name, std::function<void(char const*, size_t)> stdout_callback);

	void logScene(std::shared_ptr<XRTMirror> const &mirror, SphereLight *light);
	void logText(string const &text);
};

#endif /* INPUTOUTPUT_HPP_ */
