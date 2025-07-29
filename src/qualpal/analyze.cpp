#include "cvd.h"
#include <algorithm>
#include <qualpal/analyze.h>

namespace qualpal {

PaletteAnalysisMap
analyzePalette(const std::vector<colors::RGB>& colors,
               const metrics::MetricType& metric,
               const std::map<std::string, double>& cvd,
               const std::optional<colors::RGB>& bg,
               double max_memory)
{
  PaletteAnalysisMap result;

  auto cvd_types = cvd;
  cvd_types["normal"] = 0.0; // Ensure normal vision is included

  for (const auto& [cvd_type, severity] : cvd_types) {
    std::vector<colors::RGB> simulated_colors = colors;
    std::optional<colors::RGB> simulated_bg = bg;

    if (cvd_type != "normal" && severity > 0.0) {
      for (auto& rgb : simulated_colors) {
        rgb = simulateCvd(rgb, cvd_type, severity);
      }
      if (simulated_bg.has_value()) {
        simulated_bg = simulateCvd(*simulated_bg, cvd_type, severity);
      }
    }

    std::vector<colors::XYZ> xyz_colors;
    xyz_colors.reserve(simulated_colors.size());

    for (const auto& rgb : simulated_colors) {
      xyz_colors.emplace_back(rgb);
    }

    Matrix<double> diff_matrix =
      colorDifferenceMatrix(xyz_colors, metric, max_memory);
    std::vector<double> min_distances;
    min_distances.reserve(diff_matrix.nrow());

    for (std::size_t i = 0; i < diff_matrix.nrow(); ++i) {
      double min_dist = std::numeric_limits<double>::max();

      for (std::size_t j = 0; j < diff_matrix.ncol(); ++j) {
        if (i != j) {
          min_dist = std::min(min_dist, diff_matrix(i, j));
        }
      }

      min_distances.push_back(min_dist);
    }

    double bg_min_distance = std::numeric_limits<double>::quiet_NaN();
    if (simulated_bg.has_value()) {
      colors::XYZ bg_xyz(*simulated_bg);
      double min_dist = std::numeric_limits<double>::max();
      for (const auto& col : xyz_colors) {
        double d = 0.0;
        switch (metric) {
          case metrics::MetricType::DIN99d:
            d = metrics::DIN99d{}(col, bg_xyz);
            break;
          case metrics::MetricType::CIEDE2000:
            d = metrics::CIEDE2000{}(col, bg_xyz);
            break;
          case metrics::MetricType::CIE76:
            d = metrics::CIE76{}(col, bg_xyz);
            break;
        }
        min_dist = std::min(min_dist, d);
      }
      bg_min_distance = min_dist;
    }

    result[cvd_type] =
      PaletteAnalysis{ diff_matrix, min_distances, bg_min_distance };
  }

  return result;
}

} // namespace qualpal
