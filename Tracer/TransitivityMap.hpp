//
// Created by yalavrinenko on 09.03.18.
//

#ifndef XRAY_TRACING_LIB_TRANSITIVITYMAP_HPP
#define XRAY_TRACING_LIB_TRANSITIVITYMAP_HPP

#include <vector>
#include <string>

class TransitivityMap{
private:
    std::vector<std::vector<double>> m_map;
    std::size_t width = 0;
    std::size_t height = 0;

    struct {
        double x;
        double y;
    } pixel_size = {0.0, 0.0};

public:
    TransitivityMap() = default;

    TransitivityMap(double __psize_x, double __psize_y): pixel_size({__psize_x, __psize_y}){}

    TransitivityMap(TransitivityMap &&m) noexcept :
            pixel_size(m.pixel_size),
            width(m.width),
            height(m.height){
        m_map = std::move(m.m_map);
    }

    TransitivityMap(double __psize_x, double __psize_y, std::string const &path);

    TransitivityMap& operator= (TransitivityMap const &m) = default;

    void read_map(std::string const &path);

    /**
     *
     * @param x [-width/2..width/2]
     * @param y [-height/2..height/2]
     * @return
     */
    double transitivity (double x, double y) const;

};


#endif //XRAY_TRACING_LIB_TRANSITIVITYMAP_HPP
