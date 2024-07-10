#include "convert.h"
#include "qualpal/colors.h"
#include "qualpal/distance_matrix.h"
#include "qualpal/qualpal.h"
#include <Rcpp.h>
#include <array>

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

//' Convert colors between colorspaces
//'
//' @param colors A matrix of colors
//' @param from The colorspace of the input colors, one of "rgb", "hsl",
//'   "din99d", "lab", "xyz"
//' @param to The colorspace of the output colors,
//'   one of "rgb", "hsl", "lab", "xyz"
//'
//' @return The colors converted to the new colorspace
// [[Rcpp::export]]
Rcpp::NumericMatrix
convert_colors(const Rcpp::NumericMatrix& colors,
               const std::string& from,
               const std::string& to)
{
  int N = colors.nrow();

  Rcpp::NumericMatrix out(N, 3);
  Rcpp::CharacterVector col_names(3);

  for (int i = 0; i < N; i++) {
    std::array<double, 3> color_converted;
    if (from == "rgb") {
      qualpal::RGB rgb(colors(i, 0), colors(i, 1), colors(i, 2));
      color_converted = convert(rgb, to);
    } else if (from == "hsl") {
      qualpal::HSL hsl(colors(i, 0), colors(i, 1), colors(i, 2));
      color_converted = convert(hsl, to);
    } else if (from == "din99d") {
      Rcpp::stop("Cannot convert from din99d");
    } else if (from == "lab") {
      qualpal::Lab lab(colors(i, 0), colors(i, 1), colors(i, 2));
      color_converted = convert(lab, to);
    } else if (from == "xyz") {
      qualpal::XYZ xyz(colors(i, 0), colors(i, 1), colors(i, 2));
      color_converted = convert(xyz, to);
    } else {
      Rcpp::stop("Unknown colorspace");
    }

    out(i, 0) = color_converted[0];
    out(i, 1) = color_converted[1];
    out(i, 2) = color_converted[2];
  }

  if (to == "rgb") {
    col_names = Rcpp::CharacterVector::create("R", "G", "B");
  } else if (to == "hsl") {
    col_names = Rcpp::CharacterVector::create("H", "S", "L");
  } else if (to == "din99d") {
    col_names = Rcpp::CharacterVector::create("L(99d)", "a(99d)", "b(99d)");
  } else if (to == "lab") {
    col_names = Rcpp::CharacterVector::create("L", "a", "b");
  } else if (to == "xyz") {
    col_names = Rcpp::CharacterVector::create("X", "Y", "Z");
  } else {
    Rcpp::stop("Unknown colorspace");
  }

  colnames(out) = col_names;

  return out;
}
