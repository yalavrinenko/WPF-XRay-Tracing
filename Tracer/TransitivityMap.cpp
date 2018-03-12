//
// Created by yalavrinenko on 09.03.18.
//

#include "TransitivityMap.hpp"
#include <sstream>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <cmath>

TransitivityMap::TransitivityMap(double __psize, std::string const &path):TransitivityMap(__psize) {
    read_map(path);
}

void TransitivityMap::read_map(std::string const &path) {
    std::ifstream in (path);
    std::size_t x = 0;
    std::size_t y = 0;
    std::string line;
    while (std::getline(in, line)){
        std::vector<double> tmp_vec;

        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::copy(std::istream_iterator<std::string>(iss),
                  std::istream_iterator<std::string>(),
                  std::back_inserter(tokens));

        std::transform(tokens.begin(), tokens.end(), std::back_inserter(tmp_vec), [](std::string const &str){
            return std::stod(str);
        });

        auto tmp_size = tmp_vec.size();
        if (x != 0 && tmp_size != x){
            throw std::logic_error("Wrong Map file fromat. ["+path+"]");
        }

        x = tmp_size;

        m_map.emplace_back(std::move(tmp_vec));
        ++y;
    }

    width = x;
    height = y;
}

double TransitivityMap::transitivity(double x, double y) const {
    if (pixel_size == 0)
        throw std::logic_error("Wrong pixel size");

    long long int x_sig_idx = std::llround(x / pixel_size);
    long long int y_sig_idx = std::llround(y / pixel_size);

    std::size_t x_idx = x_sig_idx + width / 2;
    std::size_t y_idx = y_sig_idx + height / 2;

    if (x_idx >= width)
        x_idx = width - 1;
    if (y_idx >= height)
        y_idx = height - 1;

    if (x_idx < 0)
        x_idx = 0;
    if (y_idx < 0)
        y_idx = 0;

    return m_map[y_idx][x_idx];
}
