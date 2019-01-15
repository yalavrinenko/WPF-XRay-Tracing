//
// Created by yalavrinenko on 09.03.18.
//

#include "TransitivityMap.hpp"
#include <sstream>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <cmath>

TransitivityMap::TransitivityMap(double __psize_x, double __psize_y, std::string const &path):
    TransitivityMap(__psize_x, __psize_y) {
    read_map(path);
}

void TransitivityMap::read_map(std::string const &path) {
    std::ifstream in (path);
    std::string line;
    while (std::getline(in, line)){
        std::vector<double> tmp_vec;

        std::istringstream iss(line);
        std::copy(std::istream_iterator<double>(iss),
                  std::istream_iterator<double>(),
                  std::back_inserter(tmp_vec));

        m_map.emplace_back(std::move(tmp_vec));
    }

    height = m_map.size();

    if (height != 0)
        width = m_map.front().size();
    else
        throw std::logic_error("Wrong Map file fromat. ["+path+"]");

    if (std::count_if(m_map.begin(), m_map.end(), [this](auto const& v){
        return v.size() != width;
    })){
        throw std::logic_error("Wrong Map file fromat. ["+path+"]");
    }

}

double TransitivityMap::transitivity(double x, double y) const {
    throw std::string("Not inmplemented yet^") + std::string(__FUNCTION__);
    /*
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

    return m_map[y_idx][x_idx];*/
}
