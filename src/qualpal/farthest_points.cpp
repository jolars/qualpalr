#include "farthest_points.h"
#include <algorithm>
#include <limits>
#include <numeric>

namespace qualpal {

std::vector<int>
farthestPoints(const int n,
               const std::vector<colors::XYZ>& colors_in,
               const metrics::MetricType& metric_type,
               const std::optional<colors::RGB>& bg,
               const std::vector<colors::XYZ>& fixed_points,
               const double max_memory)
{
  std::vector<colors::XYZ> colors;
  colors.reserve(fixed_points.size() + colors_in.size());
  colors.insert(colors.end(), fixed_points.begin(), fixed_points.end());
  colors.insert(colors.end(), colors_in.begin(), colors_in.end());
  if (bg.has_value()) {
    colors.emplace_back(*bg);
  }

  Matrix<double> dist_mat =
    colorDifferenceMatrix(colors, metric_type, max_memory);

  std::vector<int> fixed_points_indices(fixed_points.size());

  const int n_fixed = fixed_points.size();
  const int n_candidates = colors_in.size();
  const int n_colors = n_fixed + n_candidates;

  if (n - n_fixed > n_candidates) {
    throw std::invalid_argument(
      "Requested number of new colors exceeds candidate pool.");
  }

  // Begin with the fixed points, then fill up to n with new points.
  std::vector<int> r(n);
  std::iota(r.begin(), r.end(), 0);

  // Store the complement to r (excluding fixed points).
  std::vector<int> r_c(n_candidates);
  std::iota(r_c.begin(), r_c.end(), n);

  bool set_changed = true;

  while (set_changed) {
    set_changed = false;

    for (int i = n_fixed; i < n; ++i) {
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
        min_dist_old = std::min(min_dist_old, dist_mat(r[i], n_colors));
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
          min_dist_k = std::min(min_dist_k, dist_mat(r_c[k], n_colors));
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

  for (int i = n_fixed; i < n; ++i) {
    assert(r[i] >= n_fixed &&
           "Non-candidate index found in candidate selection!");
    assert(r[i] < n_colors);
  }

  // Arrange the colors in the palette according to how distinct they are from
  // one another.
  std::sort(
    r.begin() + n_fixed, r.end(), [&dist_mat, &r, n_fixed](int a, int b) {
      double min_dist_a = std::numeric_limits<double>::max();
      double min_dist_b = std::numeric_limits<double>::max();

      for (size_t i = 0; i < r.size(); ++i) {
        if (static_cast<int>(i) >= n_fixed) {
          if (r[i] != a) {
            min_dist_a = std::min(min_dist_a, dist_mat(r[i], a));
          }
          if (r[i] != b) {
            min_dist_b = std::min(min_dist_b, dist_mat(r[i], b));
          }
        }
      }

      return min_dist_a > min_dist_b;
    });

  // Return only indices for new colors (exclude fixed points)
  std::vector<int> result;
  for (int i = n_fixed; i < n; ++i) {
    result.push_back(r[i] - n_fixed);
    assert(result.back() >= 0 &&
           "Negative index found in farthest points selection!");
  }

  return result;
}

} // namespace qualpal
