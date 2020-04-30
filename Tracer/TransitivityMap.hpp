//
// Created by yalavrinenko on 09.03.18.
//

#ifndef XRAY_TRACING_LIB_TRANSITIVITYMAP_HPP
#define XRAY_TRACING_LIB_TRANSITIVITYMAP_HPP

#include <vector>
#include <string>

class TransitivityMap{
public:
    TransitivityMap() = default;

    TransitivityMap(double psize_x, double psize_y): pixel_size({psize_x, psize_y}){
    }

    TransitivityMap(double psize_x, double psize_y, std::string const &path);

    TransitivityMap& operator= (TransitivityMap const &m) = default;

    void read_map(std::string const &path);

    /**
     *
     * @param x [-width/2..width/2]
     * @param y [-height/2..height/2]
     * @return
     */
    [[nodiscard]] double transitivity (double x, double y) const;

    [[nodiscard]] std::pair<double, double> size() const {
        return {width, height};
    }

private:
    std::vector<std::vector<double>> m_map;
    double width{0.0};
    double height{0.0};

    struct {
        double x{};
        double y{};
    } pixel_size;
};


#endif //XRAY_TRACING_LIB_TRANSITIVITYMAP_HPP
