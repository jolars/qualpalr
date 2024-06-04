#include "din99d.h"
#include "lab.h"
#include "qualpal.h"
#include "xyz.h"
#include <RcppEigen.h>

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

// [[Rcpp::export]]
Rcpp::List
qualpal_cpp(int n,
            const std::vector<double> r,
            const std::vector<double> g,
            const std::vector<double> b)
{
  int N = r.size();

  std::vector<qualpal::RGB> rgb_colors;

  for (int i = 0; i < N; ++i) {
    rgb_colors.emplace_back(r[i], g[i], b[i]);

    // qualpal::RGB rgb = rgb_colors[i];
    // qualpal::HSL hsl = rgb_colors[i];
    // qualpal::DIN99d din99d = rgb_colors[i];
    // qualpal::XYZ xyz = rgb_colors[i];
    // qualpal::Lab lab(xyz);

    // Rcpp::Rcout << "x: " << xyz.x_value() << ", y: " << xyz.y_value()
    //             << ", z: " << xyz.z_value() << "\n";

    // Rcpp::Rcout << "h: " << hsl.hue() << ", s: " << hsl.saturation()
    //             << ", l: " << hsl.lightness() << "\n";

    // Rcpp::Rcout << "l: " << lab.lightness() << ", a: " << lab.a_value()
    //             << ", b: " << lab.b_value() << "\n";

    //   Rcpp::Rcout << "l99d: " << din99d.lightness() << ", a99d: " <<
    //   din99d.a()
    //               << ", b99d: " << din99d.b() << "\n";
  }

  std::vector<qualpal::RGB> selected_colors = qualpal::qualpal(n, rgb_colors);

  std::vector<double> r_out;
  std::vector<double> g_out;
  std::vector<double> b_out;

  std::vector<double> h_out;
  std::vector<double> s_out;
  std::vector<double> l_out;

  std::vector<double> l99d_out;
  std::vector<double> a99d_out;
  std::vector<double> b99d_out;

  std::vector<std::string> hex_out;

  for (const auto& rgb : selected_colors) {
    qualpal::DIN99d din99d = rgb;
    qualpal::HSL hsl = rgb;
    std::string hex = rgb.hex();

    r_out.emplace_back(rgb.red());
    g_out.emplace_back(rgb.green());
    b_out.emplace_back(rgb.blue());

    h_out.emplace_back(hsl.hue());
    s_out.emplace_back(hsl.saturation());
    l_out.emplace_back(hsl.lightness());

    l99d_out.emplace_back(din99d.lightness());
    a99d_out.emplace_back(din99d.a());
    b99d_out.emplace_back(din99d.b());

    hex_out.emplace_back(hex);
  }

  return Rcpp::List::create(Rcpp::Named("r") = r_out,
                            Rcpp::Named("g") = g_out,
                            Rcpp::Named("b") = b_out,
                            Rcpp::Named("h") = h_out,
                            Rcpp::Named("s") = s_out,
                            Rcpp::Named("l") = l_out,
                            Rcpp::Named("l99d") = l99d_out,
                            Rcpp::Named("a99d") = a99d_out,
                            Rcpp::Named("b99d") = b99d_out,
                            Rcpp::Named("hex") = hex_out);
}
