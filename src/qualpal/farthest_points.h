#pragma once

#include <cmath>
#include <map>
#include <qualpal/color_difference.h>
#include <qualpal/colors.h>
#include <qualpal/matrix.h>
#include <string>
#include <vector>

namespace qualpal {

std::vector<std::size_t>
farthestPoints(const std::size_t n,
               const std::vector<colors::XYZ>& colors,
               const metrics::MetricType& metric_type,
               const bool has_bg = false,
               const std::size_t n_fixed = 0,
               const double max_memory = 1,
               const std::array<double, 3>& white_point = { 0.95047,
                                                            1,
                                                            1.08883 },
               const std::map<std::string, double>& cvd = {});

} // namespace qualpal
