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
class infoOut{
private:
	ofstream out;

    std::function<void(char const*, size_t)> _callback = nullptr;
public:
	infoOut();
	explicit infoOut(char const* name);
    infoOut(char const* name, std::function<void(char const*, size_t)> stdout_callback);

	void logScene(std::shared_ptr<XRTMirror> const &mirror, const unique_ptr<XRTRaySource> &light);
	void logText(string const &text);

  template<typename ... T>
  void logText(T ... args){
    std::stringstream ss;
    auto log = [&ss](auto &object) {
      ss << object << "\t";
    };

    (log(args), ...);
    logText(ss.str());
  }

  template<typename T>
  infoOut& operator << (T const &object) { logText(object); return *this; }
};

#endif /* INPUTOUTPUT_HPP_ */
