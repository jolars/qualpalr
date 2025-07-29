#include "qualpal/metrics.h"
#include <qualpal/color_difference.h>
#include <qualpal/matrix.h>

namespace qualpal {

Matrix<double>
colorDifferenceMatrix(const std::vector<colors::XYZ>& colors,
                      const metrics::MetricType& metric_type,
                      const double max_memory)
{
  switch (metric_type) {
    case metrics::MetricType::DIN99d: {
      std::vector<colors::DIN99d> din99d_colors;
      din99d_colors.reserve(colors.size());
      for (const auto& col : colors) {
        din99d_colors.emplace_back(col);
      }
      return colorDifferenceMatrix(
        din99d_colors, metrics::DIN99d{}, max_memory);
    }
    case metrics::MetricType::CIEDE2000: {
      std::vector<colors::Lab> lab_colors;
      lab_colors.reserve(colors.size());
      for (const auto& col : colors) {
        lab_colors.emplace_back(col);
      }
      return colorDifferenceMatrix(
        lab_colors, metrics::CIEDE2000{}, max_memory);
    }
    case metrics::MetricType::CIE76: {
      std::vector<colors::Lab> lab_colors;
      lab_colors.reserve(colors.size());
      for (const auto& col : colors) {
        lab_colors.emplace_back(col);
      }
      return colorDifferenceMatrix(lab_colors, metrics::CIE76{}, max_memory);
    }
  }
  throw std::invalid_argument("Unsupported metric type");
}
} // namespace qualpal
