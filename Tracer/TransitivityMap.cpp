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

    height = m_map.size() * pixel_size.y;
    width = m_map.front().size() * pixel_size.x;

    if (m_map.empty())
        throw std::logic_error("Wrong Map file fromat. ["+path+"]");

    if (std::count_if(m_map.begin(), m_map.end(), [this, w_size = m_map.front().size()](auto const& v){
        return v.size() != w_size;
    })){
        throw std::logic_error("Wrong Map file fromat. ["+path+"]");
    }
}

double TransitivityMap::transitivity(double x, double y) const {
    x += width / 2;
    y += height / 2;

    auto x_idx = static_cast<long long>(x / pixel_size.x);
    auto y_idx = static_cast<long long>(y / pixel_size.y);

    if (x_idx < 0)
        x_idx = 0;
    if (y_idx < 0)
        y_idx = 0;

    if (y_idx >= m_map.size())
        y_idx = static_cast<long long int>(m_map.size() - 1);

    if (x_idx >= m_map[y_idx].size())
        x_idx = static_cast<long long int>(m_map[y_idx].size() - 1);

    return m_map[y_idx][x_idx];
}
