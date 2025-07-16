#include "farthest_points.h"
#include <algorithm>
#include <limits>
#include <numeric>

namespace qualpal {

std::vector<int>
farthestPoints(const int n,
               const std::vector<colors::XYZ>& colors,
               const metrics::MetricType& metric_type,
               const std::optional<colors::RGB>& bg,
               const double max_memory)
{
  Matrix<double> dist_mat =
    colorDifferenceMatrix(colors, metric_type, max_memory);

  const int n_colors = colors.size();

  // Pre-compute background distances if background color is provided
  std::vector<double> bg_distances;
  if (bg.has_value()) {
    bg_distances.reserve(n_colors);
    switch (metric_type) {
      case metrics::MetricType::DIN99d: {
        auto metric = metrics::DIN99d{};
        colors::DIN99d din99d_bg(*bg);
        for (const auto& color : colors) {
          bg_distances.push_back(metric(color, din99d_bg));
        }
        break;
      }
      case metrics::MetricType::CIEDE2000: {
        auto metric = metrics::CIEDE2000{};
        colors::Lab lab_bg(*bg);
        for (const auto& color : colors) {
          bg_distances.push_back(metric(color, lab_bg));
        }
        break;
      }
      case metrics::MetricType::CIE76: {
        auto metric = metrics::CIE76{};
        colors::Lab lab_bg(*bg);
        for (const auto& color : colors) {
          bg_distances.push_back(metric(color, lab_bg));
        }
        break;
      }
    }
  }

  // Begin with the first n points.
  std::vector<int> r(n);
  std::iota(r.begin(), r.end(), 0);

  // Store the complement to r.
  std::vector<int> r_c(n_colors - n);
  std::iota(r_c.begin(), r_c.end(), n);

  bool set_changed = true;

  while (set_changed) {
    set_changed = false;

    for (int i = 0; i < n; ++i) {
      int ind_new = i;

      double min_dist_old = std::numeric_limits<double>::max();

      // Find the distance between the current point and the others in the
      // currently selected set (r).
      for (int j = 0; j < n; ++j) {
        if (j != i) {
          min_dist_old = std::min(min_dist_old, dist_mat(r[j], r[i]));
        }
      }
      if (bg.has_value()) {
        min_dist_old = std::min(min_dist_old, bg_distances[r[i]]);
      }

      bool found_better = false;

      // Check if any point in the complement set (r_c) has a greater minimum
      // distance to the points currently selected (r).
      for (int k = 0; k < n_colors - n; ++k) {
        double min_dist_k = std::numeric_limits<double>::max();

        for (int j = 0; j < n; ++j) {
          if (j != i) {
            double d = dist_mat(r[j], r_c[k]);
            min_dist_k = std::min(min_dist_k, d);
          }
        }

        if (bg.has_value()) {
          min_dist_k = std::min(min_dist_k, bg_distances[r_c[k]]);
        }

        if (min_dist_k > min_dist_old) {
          min_dist_old = min_dist_k;
          ind_new = k;
          found_better = true;
        }
      }

      // If we have found a better point in r_c, swap places with the current
      // point.
      if (found_better) {
        std::swap(r[i], r_c[ind_new]);
        set_changed = true;
      }
    }
  }

  // Arrange the colors in the palette according to how distinct they are from
  // one another.
  std::sort(r.begin(), r.end(), [&dist_mat, &r](int a, int b) {
    double min_dist_a = std::numeric_limits<double>::max();
    double min_dist_b = std::numeric_limits<double>::max();

    for (size_t i = 0; i < r.size(); ++i) {
      if (r[i] != a) {
        min_dist_a = std::min(min_dist_a, dist_mat(r[i], a));
      }
      if (r[i] != b) {
        min_dist_b = std::min(min_dist_b, dist_mat(r[i], b));
      }
    }

    return min_dist_a > min_dist_b;
  });

  return r;
}

} // namespace qualpal
