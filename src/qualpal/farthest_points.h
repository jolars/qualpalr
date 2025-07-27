#pragma once

#include <cmath>
#include <optional>
#include <qualpal/color_difference.h>
#include <qualpal/colors.h>
#include <qualpal/matrix.h>
#include <vector>

namespace qualpal {

std::vector<int>
farthestPoints(const int n,
               const std::vector<colors::XYZ>& colors,
               const metrics::MetricType& metric_type,
               const std::optional<colors::RGB>& bg = std::nullopt,
               const std::vector<colors::XYZ>& fixed_points = {},
               const double max_memory = 1);

} // namespace qualpal
