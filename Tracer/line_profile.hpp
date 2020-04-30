//
// Created by yalavrinenko on 30.04.2020.
//

#ifndef XRAY_TRACING_LIB_LINE_PROFILE_HPP
#define XRAY_TRACING_LIB_LINE_PROFILE_HPP
template<typename distribution_type>
class random_generator {
public:
  random_generator(double central_line, double sigma) :
      central_{central_line}, sigma_{sigma},
      is_constant_{std::abs(sigma) < std::numeric_limits<double>::epsilon()},
      distribution_{central_, (!is_constant_) ? sigma_ : 0.1} {
  }

  template<typename engine_type>
  double operator()(engine_type &engine) {
    if (!is_constant_)
      return distribution_(engine);
    else
      return central_;
  }

public:
  double central_;
  double sigma_;
  bool is_constant_;
  distribution_type distribution_;
};

class gauss_lineshape : public random_generator<std::normal_distribution<double>> {
public:
gauss_lineshape(double central_line, double fwhm) :
    random_generator{central_line, fwhm / (2.0 * sqrt(2.0 * log(2.0)))} {}
};

class lorentz_lineshape : public random_generator<std::cauchy_distribution<double>> {
public:
lorentz_lineshape(double central_line, double fwhm) :
    random_generator{central_line, fwhm / 2.0} {}
};
#endif //XRAY_TRACING_LIB_LINE_PROFILE_HPP
