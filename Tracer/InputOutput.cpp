/*
 * InputOutput.cpp
 *
 *  Created on: 17.02.2014
 *      Author: cheshire
 */
#include "InputOutput.hpp"

vector<Vec3d> data;

void dump(char* name, char* name_func, tRay* arr, int count, double *t) {
	ofstream out(name);
	out << "function [] = " << name_func << "()" << endl;
	out << "\tfigure" << endl;
	out << "\thold on" << endl;

	out << "[x,y,z]=sphere;" << endl;
	out << "surf(80*x,(80*y+3),(80*z+100));" << endl;

	for (int i = 0; i < count; i++) {
		Vec3d s = arr[i].b;
		Vec3d f = arr[i].trace(t[i]);
		out << "\tplot3([" << s.x << ";" << f.x << "],[" << s.y << ";" << f.y
				<< "],[" << s.z << ";" << f.z << "],'r');" << endl;
	}
	out << "end" << endl;
}

void dumpRef(char* name, char* name_func, tRay* arr, int count, double *t) {
	ofstream out(name);
	out << "function [] = " << name_func << "()" << endl;

	for (int i = 0; i < count; i++) {
		Vec3d s = arr[i].b;
		Vec3d f = arr[i].trace(t[i]);
		out << "\tplot3([" << s.x << ";" << f.x << "],[" << s.y << ";" << f.y
				<< "],[" << s.z << ";" << f.z << "],'b');" << endl;
	}
	out << "end" << endl;
}

void appPoint(Vec3d point) {
	::data.push_back(point);
}
void dumpPoint(char* name) {
	ofstream out(name);
	for (auto &i : ::data)
        out << i.x << "\t" << i.y << "\t" << i.z << endl;
}

string doubleToStr(double a) {
	return std::to_string(a);
}

string intToStr(int a) {
	return std::to_string(a);
}

void dumpPlane(char* name, vector<Vec3d> odata) {
	ofstream out(name);
	for (auto &i : odata)
        out << i.x << "\t" << i.y << "\t" << i.z << endl;
}

void dumpRoad(char* name, char* name_func, vector<vector<Vec3d> > const &road) {
	ofstream out(name);
	out << "function [] = " << name_func << "()" << endl;
	out << "\tfigure" << endl;
	out << "\thold on" << endl;

	out << "[x,y,z]=sphere;" << endl;
	out << "surf(80*x,(80*y+100),(80*z));" << endl;

	for (auto &i : road) {
		string x = "[";
		string y = "[";
		string z = "[";
		for (const auto & j : i) {
			x += doubleToStr(j.x) + ";";
			y += doubleToStr(j.y) + ";";
			z += doubleToStr(j.z) + ";";
		}
		x += "]";
		y += "]";
		z += "]";
		out << "plot3(" << x << "," << y << "," << z << ");" << endl;
	}

	out << "end" << endl;
}

void dumpRoadNOSPH(char const* name, char const* name_func, vector<vector<Vec3d> > const &road) {
	ofstream out(name);
	out << "function [] = " << name_func << "()" << endl;
	out << "\tfigure" << endl;
	out << "\thold on" << endl;

	for (auto &i : road) {
		string x = "[";
		string y = "[";
		string z = "[";
		for (auto & j : i) {
			x += doubleToStr(j.x) + ";";
			y += doubleToStr(j.y) + ";";
			z += doubleToStr(j.z) + ";";
		}
		x += "]";
		y += "]";
		z += "]";
		out << "plot3(" << x << "," << y << "," << z << ");" << endl;
	}

	out << "end" << endl;
}

void dumpRays(tRay* d,char* fileName,int count, Vec3d dir){
	ofstream off(fileName);
	for (int i=0; i<count; i++){
		Vec3d direction = d[i].k / sqrt(dot(d[i].k));
		double ang=acos(dot2(dir, direction) / (sqrt(dot(dir)) * sqrt(dot(direction))));;
		off<<d[i].k.x<<"\t"<<d[i].k.y<<"\t"<<d[i].k.z<<"\t"<<ang*180/M_PI<<endl;
	}
	off.close();
}

infoOut::infoOut():
        infoOut("log.txt", nullptr){

}

infoOut::infoOut(char const* name):
        infoOut(name, nullptr){
}

infoOut::infoOut(char const* name, std::function<void(char const*, size_t)> stdout_callback): _callback(stdout_callback){
    out.open(name);
}

void infoOut::logScene(std::shared_ptr<XRTMirror> const &mirror, std::shared_ptr<XRTRaySource> const &light) {
	auto r0 = mirror->GetR0();
	out << "[MIRROR]" << endl;
	out << "[X,Y,Z]" << "[" << r0.x << ", " << r0.y << ", "
			<<r0.z << "]" << endl << endl;
	out << "[LIGHT SOURCE]" << endl;
	out << "[X,Y,Z]" << "[" << light->GetR0().x << ", " << light->GetR0().y
			<< ", " << light->GetR0().z << "]" << endl;
}

void infoOut::logText(string const &text) {
	out << text << endl;
	if (_callback){
        _callback(text.c_str(), text.length());
    }
}
