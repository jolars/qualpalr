#include "convert.h"
#include "qualpal/palettes.h"
#include <Rcpp.h>
#include <array>
#include <qualpal.h>

template<typename T>
Rcpp::NumericMatrix
convert_matrix(const T& mat)
{
  int n = mat.nrow();
  int m = mat.ncol();

  Rcpp::NumericMatrix out(n, m);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      out(i, j) = mat(i, j);
    }
  }

  return out;
}

bool
contains(const Rcpp::List& list, const std::string& s)
{
  Rcpp::CharacterVector list_names = list.names();
  for (size_t i = 0; i < list_names.size(); i++) {
    if (std::string(list_names[i]) == s) {
      return true;
    }
  }
  return false;
}

qualpal::Qualpal
setup_palette(const Rcpp::List& options)
{
  using Rcpp::as;

  qualpal::Qualpal qp;

  auto bg_vec = as<Rcpp::NumericVector>(options["bg"]);
  auto cvd_list = as<Rcpp::List>(options["cvd"]);
  auto metric = as<std::string>(options["metric"]);
  auto white_point_str = as<std::string>(options["white_point"]);

  if (bg_vec.size() == 3) {
    qp.setBackground(qualpal::colors::RGB(bg_vec[0], bg_vec[1], bg_vec[2]));
  }

  if (cvd_list.size() > 0) {
    std::map<std::string, double> cvd;

    const std::vector<std::string> cvd_types = { "protan", "deutan", "tritan" };
    for (const auto& type : cvd_types) {
      if (contains(cvd_list, type)) {
        cvd[type] = as<double>(cvd_list[type]);
      }
    }

    qp.setCvd(cvd);
  }

  if (metric == "din99d") {
    qp.setMetric(qualpal::metrics::MetricType::DIN99d);
  } else if (metric == "ciede2000") {
    qp.setMetric(qualpal::metrics::MetricType::CIEDE2000);
  } else if (metric == "cie76") {
    qp.setMetric(qualpal::metrics::MetricType::CIE76);
  }

  if (white_point_str == "D65") {
    qp.setWhitePoint(qualpal::WhitePoint::D65);
  } else if (white_point_str == "D50") {
    qp.setWhitePoint(qualpal::WhitePoint::D50);
  } else if (white_point_str == "D55") {
    qp.setWhitePoint(qualpal::WhitePoint::D55);
  } else if (white_point_str == "A") {
    qp.setWhitePoint(qualpal::WhitePoint::A);
  } else if (white_point_str == "E") {
    qp.setWhitePoint(qualpal::WhitePoint::E);
  }

  return qp;
}

Rcpp::List
organize_output(const std::vector<qualpal::colors::RGB> colors)
{
  using namespace qualpal::colors;

  int n = colors.size();

  Rcpp::NumericMatrix rgb_out(n, 3);
  Rcpp::NumericMatrix hsl_out(n, 3);
  Rcpp::NumericMatrix din99d_out(n, 3);

  std::vector<DIN99d> colors_din99d;

  std::vector<std::string> hex_out;

  for (int i = 0; i < n; ++i) {
    RGB rgb = colors[i];
    DIN99d din99d(rgb);
    HSL hsl(rgb);
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

  auto dist_mat =
    qualpal::colorDifferenceMatrix(colors_din99d, qualpal::metrics::DIN99d{});

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
                const Rcpp::List& options)
{
  int N = rgb_in.nrow();

  std::vector<qualpal::colors::RGB> rgb_colors;

  for (int i = 0; i < N; ++i) {
    rgb_colors.emplace_back(rgb_in(i, 0), rgb_in(i, 1), rgb_in(i, 2));
  }

  auto qp = setup_palette(options);
  qp.setInputRGB(rgb_colors);

  auto extend = Rcpp::as<Rcpp::NumericMatrix>(options["extend"]);

  std::vector<qualpal::colors::RGB> selected_colors;

  if (extend.nrow() > 0) {
    std::vector<qualpal::colors::RGB> base_colors;
    for (int i = 0; i < extend.nrow(); ++i) {
      base_colors.emplace_back(extend(i, 0), extend(i, 1), extend(i, 2));
    }
    selected_colors = qp.extend(base_colors, n);
  } else {
    selected_colors = qp.generate(n);
  }

  return organize_output(selected_colors);
}

// [[Rcpp::export]]
Rcpp::List
qualpal_cpp_colorspace(int n,
                       const Rcpp::List& colorspace,
                       const int n_points,
                       const Rcpp::List& options)
{
  auto qp = setup_palette(options);
  auto extend = Rcpp::as<Rcpp::NumericMatrix>(options["extend"]);

  std::vector<double> h_lim_vec =
    Rcpp::as<std::vector<double>>(colorspace["h"]);
  std::vector<double> s_or_c_lim_vec =
    Rcpp::as<std::vector<double>>(colorspace["s_or_c"]);
  std::vector<double> l_lim_vec =
    Rcpp::as<std::vector<double>>(colorspace["l"]);

  std::array<double, 2> h_lim = { h_lim_vec[0], h_lim_vec[1] };
  std::array<double, 2> s_or_c_lim = { s_or_c_lim_vec[0], s_or_c_lim_vec[1] };
  std::array<double, 2> l_lim = { l_lim_vec[0], l_lim_vec[1] };

  std::string colorspace_type = Rcpp::as<std::string>(colorspace["type"]);

  if (colorspace_type == "hsl") {
    qp.setInputColorspace(
      h_lim, s_or_c_lim, l_lim, qualpal::ColorspaceType::HSL);
  } else if (colorspace_type == "lchab") {
    qp.setInputColorspace(
      h_lim, s_or_c_lim, l_lim, qualpal::ColorspaceType::LCHab);
  }

  qp.setColorspaceSize(n_points);

  std::vector<qualpal::colors::RGB> selected_colors;

  if (extend.nrow() > 0) {
    std::vector<qualpal::colors::RGB> base_colors;
    for (int i = 0; i < extend.nrow(); ++i) {
      base_colors.emplace_back(extend(i, 0), extend(i, 1), extend(i, 2));
    }
    selected_colors = qp.extend(base_colors, n);
  } else {
    selected_colors = qp.generate(n);
  }

  return organize_output(selected_colors);
}

// [[Rcpp::export]]
Rcpp::List
qualpal_cpp_palette(int n,
                    const std::string& palette,
                    const Rcpp::List& options)
{
  auto extend = Rcpp::as<Rcpp::NumericMatrix>(options["extend"]);
  auto qp = setup_palette(options);
  qp.setInputPalette(palette);

  std::vector<qualpal::colors::RGB> selected_colors;

  if (extend.nrow() > 0) {
    std::vector<qualpal::colors::RGB> base_colors;
    for (int i = 0; i < extend.nrow(); ++i) {
      base_colors.emplace_back(extend(i, 0), extend(i, 1), extend(i, 2));
    }
    selected_colors = qp.extend(base_colors, n);
  } else {
    selected_colors = qp.generate(n);
  }

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
  using namespace Rcpp;
  using namespace qualpal::colors;

  int N = colors.nrow();

  Rcpp::NumericMatrix out(N, 3);
  Rcpp::CharacterVector col_names(3);

  for (int i = 0; i < N; i++) {
    std::array<double, 3> color_converted;
    if (from == "rgb") {
      RGB rgb(colors(i, 0), colors(i, 1), colors(i, 2));
      color_converted = convert(rgb, to);
    } else if (from == "hsl") {
      HSL hsl(colors(i, 0), colors(i, 1), colors(i, 2));
      color_converted = convert(hsl, to);
    } else if (from == "din99d") {
      Rcpp::stop("Cannot convert from din99d");
    } else if (from == "lab") {
      Lab lab(colors(i, 0), colors(i, 1), colors(i, 2));
      color_converted = convert(lab, to);
    } else if (from == "xyz") {
      XYZ xyz(colors(i, 0), colors(i, 1), colors(i, 2));
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

Rcpp::List
wrap_analyzis_output(const qualpal::PaletteAnalysis& result,
                     const std::vector<std::string>& hex_colors)
{
  Rcpp::List out;

  Rcpp::NumericMatrix de_matrix = convert_matrix(result.difference_matrix);
  Rcpp::NumericVector min_distances = Rcpp::wrap(result.min_distances);
  double bg_min_distance = result.bg_min_distance;

  de_matrix.attr("dimnames") = Rcpp::List::create(hex_colors, hex_colors);
  min_distances.attr("names") = hex_colors;

  return Rcpp::List::create(Rcpp::Named("difference_matrix") = de_matrix,
                            Rcpp::Named("min_distances") = min_distances,
                            Rcpp::Named("bg_min_distance") = bg_min_distance);
}

// [[Rcpp::export]]
Rcpp::List
analyze_palette_cpp(const Rcpp::NumericMatrix& rgb_in,
                    const Rcpp::List& options)
{
  using Rcpp::as;

  int n = rgb_in.nrow();

  std::vector<qualpal::colors::RGB> rgb_colors;
  std::vector<std::string> hex_colors;

  for (int i = 0; i < n; ++i) {
    qualpal::colors::RGB rgb(rgb_in(i, 0), rgb_in(i, 1), rgb_in(i, 2));
    rgb_colors.emplace_back(rgb);
    hex_colors.emplace_back(rgb.hex());
  }

  auto bg_vec = as<Rcpp::NumericVector>(options["bg"]);
  auto cvd_list = as<Rcpp::List>(options["cvd"]);
  auto metric = as<std::string>(options["metric"]);

  std::optional<qualpal::colors::RGB> bg = std::nullopt;

  if (bg_vec.size() == 3) {
    bg = qualpal::colors::RGB(bg_vec[0], bg_vec[1], bg_vec[2]);
  }

  std::map<std::string, double> cvd;

  if (cvd_list.size() > 0) {
    const std::vector<std::string> cvd_types = { "protan", "deutan", "tritan" };
    for (const auto& type : cvd_types) {
      if (contains(cvd_list, type)) {
        cvd[type] = as<double>(cvd_list[type]);
      }
    }
  }
  qualpal::PaletteAnalysisMap result;

  if (metric == "din99d") {
    result = qualpal::analyzePalette(
      rgb_colors, qualpal::metrics::MetricType::DIN99d, cvd, bg);
  } else if (metric == "ciede2000") {
    result = qualpal::analyzePalette(
      rgb_colors, qualpal::metrics::MetricType::CIEDE2000, cvd, bg);
  } else if (metric == "cie76") {
    result = qualpal::analyzePalette(
      rgb_colors, qualpal::metrics::MetricType::CIE76, cvd, bg);
  } else {
    Rcpp::stop("Unknown metric type: " + metric);
  }

  Rcpp::List out;

  for (const auto& [name, analysis] : result) {
    out[name] = wrap_analyzis_output(analysis, hex_colors);
  }

  // out["hex"] = hex_colors;

  return out;
}

// [[Rcpp::export]]
std::map<std::string, std::vector<std::string>>
list_palettes_cpp()
{
  std::map<std::string, std::vector<std::string>> palettes =
    qualpal::listAvailablePalettes();

  return palettes;
}

// [[Rcpp::export]]
std::vector<std::string>
get_palette_cpp(const std::string& palette)
{
  std::vector<std::string> colors = qualpal::getPalette(palette);
  return colors;
}
