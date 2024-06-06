#include "qualpal/colors.h"
#include "qualpal/distance_matrix.h"
#include "qualpal/matrix.h"
#include "qualpal/qualpal.h"
#include <Rcpp.h>

Rcpp::List
organize_output(const std::vector<qualpal::RGB> colors)
{

  int n = colors.size();

  Rcpp::NumericMatrix rgb_out(n, 3);
  Rcpp::NumericMatrix hsl_out(n, 3);
  Rcpp::NumericMatrix din99d_out(n, 3);

  std::vector<qualpal::DIN99d> colors_din99d;

  std::vector<std::string> hex_out;

  // for (const auto& rgb : selected_colors) {
  for (int i = 0; i < n; ++i) {
    qualpal::RGB rgb = colors[i];
    qualpal::DIN99d din99d(rgb);
    qualpal::HSL hsl(rgb);
    std::string hex = rgb.hex();

    colors_din99d.emplace_back(din99d);

    rgb_out(i, 0) = rgb.r();
    rgb_out(i, 1) = rgb.g();
    rgb_out(i, 2) = rgb.b();

    hsl_out(i, 0) = hsl.h();
    hsl_out(i, 1) = hsl.s();
    hsl_out(i, 2) = hsl.l();

    din99d_out(i, 0) = din99d.l();
    din99d_out(i, 1) = din99d.a();
    din99d_out(i, 2) = din99d.b();

    hex_out.emplace_back(hex);
  }

  auto dist_mat = qualpal::distanceMatrix(colors_din99d);

  Rcpp::NumericMatrix de_DIN99d(n, n);
  double min_de_DIN99d = 1e9;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      de_DIN99d(i, j) = dist_mat(i, j);
      min_de_DIN99d = std::min(min_de_DIN99d, dist_mat(i, j));
    }
  }

  hsl_out.attr("dimnames") = Rcpp::List::create(
    hex_out, Rcpp::CharacterVector::create("Hue", "Saturation", "Lightness"));

  rgb_out.attr("dimnames") = Rcpp::List::create(
    hex_out, Rcpp::CharacterVector::create("Red", "Green", "Blue"));

  din99d_out.attr("dimnames") = Rcpp::List::create(
    hex_out, Rcpp::CharacterVector::create("L(99d)", "a(99d)", "b(99d)"));

  de_DIN99d.attr("dimnames") = Rcpp::List::create(hex_out, hex_out);

  Rcpp::List out =
    Rcpp::List::create(Rcpp::Named("HSL") = hsl_out,
                       Rcpp::Named("RGB") = rgb_out,
                       Rcpp::Named("DIN99d") = din99d_out,
                       Rcpp::Named("hex") = hex_out,
                       Rcpp::Named("de_DIN99d") = de_DIN99d,
                       Rcpp::Named("min_de_DIN99d") = min_de_DIN99d);

  out.attr("class") = Rcpp::CharacterVector::create("qualpal", "list");

  return out;
}

// [[Rcpp::export]]
Rcpp::List
qualpal_cpp_rgb(int n,
                const Rcpp::NumericMatrix& rgb_in,
                const Rcpp::List& cvd_list)
{
  int N = rgb_in.nrow();

  std::vector<qualpal::RGB> rgb_colors;

  for (int i = 0; i < N; ++i) {
    rgb_colors.emplace_back(rgb_in(i, 0), rgb_in(i, 1), rgb_in(i, 2));
  }

  std::map<std::string, double> cvd;

  cvd["protan"] = Rcpp::as<double>(cvd_list["protan"]);
  cvd["deutan"] = Rcpp::as<double>(cvd_list["deutan"]);
  cvd["tritan"] = Rcpp::as<double>(cvd_list["tritan"]);

  std::vector<qualpal::RGB> selected_colors =
    qualpal::qualpal(n, rgb_colors, cvd);

  return organize_output(selected_colors);
}

// [[Rcpp::export]]
Rcpp::List
qualpal_cpp_colorspace(int n,
                       const Rcpp::List& hsl_colorspace,
                       const int n_points,
                       const Rcpp::List& cvd_list)
{
  std::map<std::string, double> cvd;

  cvd["protan"] = Rcpp::as<double>(cvd_list["protan"]);
  cvd["deutan"] = Rcpp::as<double>(cvd_list["deutan"]);
  cvd["tritan"] = Rcpp::as<double>(cvd_list["tritan"]);

  std::vector<double> h_lim_vec =
    Rcpp::as<std::vector<double>>(hsl_colorspace["h"]);
  std::vector<double> s_lim_vec =
    Rcpp::as<std::vector<double>>(hsl_colorspace["s"]);
  std::vector<double> l_lim_vec =
    Rcpp::as<std::vector<double>>(hsl_colorspace["l"]);

  std::array<double, 2> h_lim = { h_lim_vec[0], h_lim_vec[1] };
  std::array<double, 2> s_lim = { s_lim_vec[0], s_lim_vec[1] };
  std::array<double, 2> l_lim = { l_lim_vec[0], l_lim_vec[1] };

  std::vector<qualpal::RGB> selected_colors =
    qualpal::qualpal(n, h_lim, s_lim, l_lim, n_points, cvd);

  return organize_output(selected_colors);
}
