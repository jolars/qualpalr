#include "farthest_points.h"
#include <algorithm>
#include <limits>
#include <numeric>

namespace qualpal {

std::vector<std::size_t>
farthestPoints(const std::size_t n,
               const std::vector<colors::XYZ>& colors,
               const metrics::MetricType& metric_type,
               const bool has_bg,
               const std::size_t n_fixed,
               const double max_memory)
{
  Matrix<double> dist_mat =
    colorDifferenceMatrix(colors, metric_type, max_memory);

  const std::size_t n_candidates = colors.size() - n_fixed - (has_bg ? 1 : 0);
  const std::size_t n_colors = colors.size();

  if (n - n_fixed > n_candidates) {
    throw std::invalid_argument(
      "Requested number of new colors exceeds candidate pool.");
  }

  // Begin with the fixed points, then fill up to n with new points.
  std::vector<std::size_t> r(n);
  std::iota(r.begin(), r.end(), 0);

  // Store the complement to r (excluding fixed points).
  std::vector<std::size_t> r_c(colors.size() - n);
  std::iota(r_c.begin(), r_c.end(), n);

  bool set_changed = true;

  while (set_changed) {
    set_changed = false;

    for (std::size_t i = n_fixed; i < n; ++i) {
      std::size_t ind_new = i;

      double min_dist_old = std::numeric_limits<double>::max();

      // Find the distance between the current point and the others in the
      // currently selected set (r).
      for (std::size_t j = 0; j < n; ++j) {
        assert(r[j] < n_colors && "Index out of bounds in r[j]");
        if (j != i) {
          min_dist_old = std::min(min_dist_old, dist_mat(r[j], r[i]));
        }
      }

      if (has_bg) {
        min_dist_old = std::min(min_dist_old, dist_mat(r[i], n_colors - 1));
      }

      bool found_better = false;

      // Check if any point in the complement set (r_c) has a greater minimum
      // distance to the points currently selected (r).
      for (std::size_t k = 0; k < r_c.size(); ++k) {
        double min_dist_k = std::numeric_limits<double>::max();

        for (std::size_t j = 0; j < n; ++j) {
          if (j != i) {
            double d = dist_mat(r[j], r_c[k]);
            min_dist_k = std::min(min_dist_k, d);
          }
        }

        if (has_bg) {
          min_dist_k = std::min(min_dist_k, dist_mat(r_c[k], n_colors - 1));
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

  for (std::size_t i = n_fixed; i < n; ++i) {
    assert(r[i] >= n_fixed &&
           "Non-candidate index found in candidate selection!");
  }

  // Arrange the colors in the palette according to how distinct they are from
  // one another.
  std::sort(r.begin() + n_fixed,
            r.end(),
            [&dist_mat, &r, n_fixed](std::size_t a, std::size_t b) {
              double min_dist_a = std::numeric_limits<double>::max();
              double min_dist_b = std::numeric_limits<double>::max();

              for (size_t i = 0; i < r.size(); ++i) {
                if (i >= n_fixed) {
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

  return r;
}

} // namespace qualpal
