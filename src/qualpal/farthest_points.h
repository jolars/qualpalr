#pragma once

#include <cmath>
#include <qualpal/color_difference.h>
#include <qualpal/colors.h>
#include <qualpal/matrix.h>
#include <vector>

namespace qualpal {

std::vector<std::size_t>
farthestPoints(const std::size_t n,
               const std::vector<colors::XYZ>& colors,
               const metrics::MetricType& metric_type,
               const bool has_bg = false,
               const std::size_t n_fixed = 0,
               const double max_memory = 1);

} // namespace qualpal
