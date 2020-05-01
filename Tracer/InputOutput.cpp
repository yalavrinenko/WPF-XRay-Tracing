/*
 * InputOutput.cpp
 *
 *  Created on: 17.02.2014
 *      Author: cheshire
 */
#include "InputOutput.hpp"

vector<Vec3d> data;

infoOut::infoOut():
        infoOut("log.txt", nullptr){
}

infoOut::infoOut(char const* name):
        infoOut(name, nullptr){
}

infoOut::infoOut(char const* name, std::function<void(char const*, size_t)> stdout_callback): _callback(std::move(stdout_callback)){
    out.open(name);
}

void infoOut::logScene(std::shared_ptr<XRTMirror> const &mirror,
                       const unique_ptr<XRTRaySource> &light) {
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
