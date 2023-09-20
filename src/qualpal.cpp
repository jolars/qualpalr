#include <RcppEigen.h>
#include <cmath>
#include <limits>
#include <numeric>
#include <unordered_set>
#include <vector>
#include <algorithm>

// [[Rcpp::export]]
Eigen::MatrixXd
edist(const Eigen::MatrixXd& x)
{
  using namespace Eigen;

  const int N = x.rows();
  const MatrixXd x_t = x.transpose();
  MatrixXd result(N, N);

  for (int i = 0; i < N; ++i) {
    for (int j = i; j < N; ++j) {
      double d = (x_t.col(i) - x_t.col(j)).norm();
      d = std::pow(d, 0.74) * 1.28;
      result(i, j) = d;
      result(j, i) = d;
    }
  }

  return result;
}

// [[Rcpp::export]]
std::vector<int>
farthest_points(const Eigen::MatrixXd& data, const int n)
{
  Eigen::MatrixXd dist_mat = edist(data);
  const int N = dist_mat.cols();

  // Begin with the first n points.
  std::vector<int> r(n);
  std::iota(r.begin(), r.end(), 0);

  // Store the complement to r.
  std::vector<int> r_c(N - n);
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

      // Check if any point in the complement set (r_c) has a greater minimum
      // distance to the points currently selected (r).
      for (int k = 0; k < N - n; ++k) {
        double min_dist_k = std::numeric_limits<double>::max();

        for (int j = 0; j < n; ++j) {
          if (j != i) {
            double d = dist_mat(r[j], r_c[k]);
            min_dist_k = std::min(min_dist_k, d);
          }
        }

        if (min_dist_k > min_dist_old) {
          min_dist_old = min_dist_k;
          ind_new = k;
        }
      }

      // If we have found a better point in r_c, swap places with the current
      // point.
      if (ind_new != i) {
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
