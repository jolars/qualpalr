#include "convert.h"
#include "qualpal/palettes.h"
#include <array>
#include <cpp11.hpp>
#include <qualpal.h>

template<typename T>
cpp11::writable::doubles_matrix<>
convert_matrix(const T& mat)
{
  int n = mat.nrow();
  int m = mat.ncol();

  cpp11::writable::doubles_matrix<> out(n, m);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      out(i, j) = mat(i, j);
    }
  }

  return out;
}

bool
contains(const cpp11::list& list, const std::string& s)
{
  cpp11::strings list_names = list.names();
  for (size_t i = 0; i < list_names.size(); i++) {
    if (std::string(list_names[i]) == s) {
      return true;
    }
  }
  return false;
}

qualpal::Qualpal
setup_palette(const cpp11::list& options)
{
  qualpal::Qualpal qp;

  auto bg_vec = cpp11::as_cpp<cpp11::doubles>(options["bg"]);
  auto cvd_list = cpp11::as_cpp<cpp11::list>(options["cvd"]);
  auto metric = cpp11::as_cpp<std::string>(options["metric"]);
  auto white_point_str = cpp11::as_cpp<std::string>(options["white_point"]);

  if (bg_vec.size() == 3) {
    qp.setBackground(qualpal::colors::RGB(bg_vec[0], bg_vec[1], bg_vec[2]));
  }

  if (cvd_list.size() > 0) {
    std::map<std::string, double> cvd;

    const std::vector<std::string> cvd_types = { "protan", "deutan", "tritan" };
    for (const auto& type : cvd_types) {
      if (contains(cvd_list, type)) {
        cvd[type] = cpp11::as_cpp<double>(cvd_list[type]);
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

cpp11::writable::list
organize_output(const std::vector<qualpal::colors::RGB> colors)
{
  using namespace qualpal::colors;

  int n = colors.size();

  cpp11::writable::doubles_matrix<> rgb_out(n, 3);
  cpp11::writable::doubles_matrix<> hsl_out(n, 3);
  cpp11::writable::doubles_matrix<> din99d_out(n, 3);

  std::vector<DIN99d> colors_din99d;

  cpp11::writable::strings hex_out(n);

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

    hex_out[i] = hex;
  }

  auto dist_mat =
    qualpal::colorDifferenceMatrix(colors_din99d, qualpal::metrics::DIN99d{});

  cpp11::writable::doubles_matrix<> de_DIN99d(n, n);
  double min_de_DIN99d = 1e9;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      de_DIN99d(i, j) = dist_mat(i, j);
      min_de_DIN99d = std::min(min_de_DIN99d, dist_mat(i, j));
    }
  }

  cpp11::writable::strings hsl_cols({ "Hue", "Saturation", "Lightness" });
  cpp11::writable::strings rgb_cols({ "Red", "Green", "Blue" });
  cpp11::writable::strings din99d_cols({ "L(99d)", "a(99d)", "b(99d)" });

  hsl_out.attr("dimnames") = cpp11::writable::list({ hex_out, hsl_cols });
  rgb_out.attr("dimnames") = cpp11::writable::list({ hex_out, rgb_cols });
  din99d_out.attr("dimnames") = cpp11::writable::list({ hex_out, din99d_cols });
  de_DIN99d.attr("dimnames") = cpp11::writable::list({ hex_out, hex_out });

  cpp11::writable::list out(
    { cpp11::named_arg("HSL") = hsl_out,
      cpp11::named_arg("RGB") = rgb_out,
      cpp11::named_arg("DIN99d") = din99d_out,
      cpp11::named_arg("hex") = hex_out,
      cpp11::named_arg("de_DIN99d") = de_DIN99d,
      cpp11::named_arg("min_de_DIN99d") = min_de_DIN99d });

  out.attr("class") = cpp11::writable::strings({ "qualpal", "list" });

  return out;
}

[[cpp11::register]]
cpp11::writable::list
qualpal_cpp_rgb(int n, cpp11::doubles_matrix<> rgb_in, cpp11::list options)
{
  int N = rgb_in.nrow();

  std::vector<qualpal::colors::RGB> rgb_colors;

  for (int i = 0; i < N; ++i) {
    rgb_colors.emplace_back(rgb_in(i, 0), rgb_in(i, 1), rgb_in(i, 2));
  }

  auto qp = setup_palette(options);
  qp.setInputRGB(rgb_colors);

  auto extend = cpp11::as_cpp<cpp11::doubles_matrix<>>(options["extend"]);

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

[[cpp11::register]]
cpp11::writable::list
qualpal_cpp_colorspace(int n,
                       cpp11::list colorspace,
                       int n_points,
                       cpp11::list options)
{
  auto qp = setup_palette(options);
  auto extend = cpp11::as_cpp<cpp11::doubles_matrix<>>(options["extend"]);

  std::vector<double> h_lim_vec =
    cpp11::as_cpp<std::vector<double>>(colorspace["h"]);
  std::vector<double> s_or_c_lim_vec =
    cpp11::as_cpp<std::vector<double>>(colorspace["s_or_c"]);
  std::vector<double> l_lim_vec =
    cpp11::as_cpp<std::vector<double>>(colorspace["l"]);

  std::array<double, 2> h_lim = { h_lim_vec[0], h_lim_vec[1] };
  std::array<double, 2> s_or_c_lim = { s_or_c_lim_vec[0], s_or_c_lim_vec[1] };
  std::array<double, 2> l_lim = { l_lim_vec[0], l_lim_vec[1] };

  std::string colorspace_type = cpp11::as_cpp<std::string>(colorspace["type"]);

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

[[cpp11::register]]
cpp11::writable::list
qualpal_cpp_palette(int n, std::string palette, cpp11::list options)
{
  auto extend = cpp11::as_cpp<cpp11::doubles_matrix<>>(options["extend"]);
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

[[cpp11::register]]
cpp11::writable::doubles_matrix<>
convert_colors_cpp(cpp11::doubles_matrix<> colors,
                   std::string from,
                   std::string to)
{
  using namespace qualpal::colors;

  int N = colors.nrow();

  cpp11::writable::doubles_matrix<> out(N, 3);

  for (int i = 0; i < N; i++) {
    std::array<double, 3> color_converted;
    if (from == "rgb") {
      RGB rgb(colors(i, 0), colors(i, 1), colors(i, 2));
      color_converted = convert(rgb, to);
    } else if (from == "hsl") {
      HSL hsl(colors(i, 0), colors(i, 1), colors(i, 2));
      color_converted = convert(hsl, to);
    } else if (from == "din99d") {
      cpp11::stop("Cannot convert from din99d");
    } else if (from == "lab") {
      Lab lab(colors(i, 0), colors(i, 1), colors(i, 2));
      color_converted = convert(lab, to);
    } else if (from == "xyz") {
      XYZ xyz(colors(i, 0), colors(i, 1), colors(i, 2));
      color_converted = convert(xyz, to);
    } else {
      cpp11::stop("Unknown colorspace");
    }

    out(i, 0) = color_converted[0];
    out(i, 1) = color_converted[1];
    out(i, 2) = color_converted[2];
  }

  cpp11::writable::strings col_names;
  if (to == "rgb") {
    col_names = cpp11::writable::strings({ "R", "G", "B" });
  } else if (to == "hsl") {
    col_names = cpp11::writable::strings({ "H", "S", "L" });
  } else if (to == "din99d") {
    col_names = cpp11::writable::strings({ "L(99d)", "a(99d)", "b(99d)" });
  } else if (to == "lab") {
    col_names = cpp11::writable::strings({ "L", "a", "b" });
  } else if (to == "xyz") {
    col_names = cpp11::writable::strings({ "X", "Y", "Z" });
  } else {
    cpp11::stop("Unknown colorspace");
  }

  out.attr("dimnames") = cpp11::writable::list({ R_NilValue, col_names });

  return out;
}

cpp11::writable::list
wrap_analyzis_output(const qualpal::PaletteAnalysis& result,
                     const cpp11::writable::strings& hex_colors)
{
  cpp11::writable::doubles_matrix<> de_matrix =
    convert_matrix(result.difference_matrix);
  cpp11::writable::doubles min_distances = cpp11::as_sexp(result.min_distances);
  double bg_min_distance = result.bg_min_distance;

  de_matrix.attr("dimnames") =
    cpp11::writable::list({ hex_colors, hex_colors });
  min_distances.attr("names") = hex_colors;

  return cpp11::writable::list(
    { cpp11::named_arg("difference_matrix") = de_matrix,
      cpp11::named_arg("min_distances") = min_distances,
      cpp11::named_arg("bg_min_distance") = bg_min_distance });
}

[[cpp11::register]]
cpp11::writable::list
analyze_palette_cpp(cpp11::doubles_matrix<> rgb_in, cpp11::list options)
{
  int n = rgb_in.nrow();

  std::vector<qualpal::colors::RGB> rgb_colors;
  cpp11::writable::strings hex_colors(n);

  for (int i = 0; i < n; ++i) {
    qualpal::colors::RGB rgb(rgb_in(i, 0), rgb_in(i, 1), rgb_in(i, 2));
    rgb_colors.emplace_back(rgb);
    hex_colors[i] = rgb.hex();
  }

  auto bg_vec = cpp11::as_cpp<cpp11::doubles>(options["bg"]);
  auto cvd_list = cpp11::as_cpp<cpp11::list>(options["cvd"]);
  auto metric = cpp11::as_cpp<std::string>(options["metric"]);

  std::optional<qualpal::colors::RGB> bg = std::nullopt;

  if (bg_vec.size() == 3) {
    bg = qualpal::colors::RGB(bg_vec[0], bg_vec[1], bg_vec[2]);
  }

  std::map<std::string, double> cvd;

  if (cvd_list.size() > 0) {
    const std::vector<std::string> cvd_types = { "protan", "deutan", "tritan" };
    for (const auto& type : cvd_types) {
      if (contains(cvd_list, type)) {
        cvd[type] = cpp11::as_cpp<double>(cvd_list[type]);
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
    cpp11::stop("Unknown metric type: " + metric);
  }

  cpp11::writable::list out;

  for (const auto& [name, analysis] : result) {
    out.push_back(cpp11::named_arg(name.c_str()) =
                    wrap_analyzis_output(analysis, hex_colors));
  }

  return out;
}

[[cpp11::register]]
cpp11::writable::list
list_palettes_cpp()
{
  std::map<std::string, std::vector<std::string>> palettes =
    qualpal::listAvailablePalettes();

  cpp11::writable::list out;
  for (const auto& [name, colors] : palettes) {
    out.push_back(cpp11::named_arg(name.c_str()) = cpp11::as_sexp(colors));
  }

  return out;
}

[[cpp11::register]]
cpp11::writable::strings
get_palette_cpp(std::string palette)
{
  std::vector<std::string> colors = qualpal::getPalette(palette);
  return cpp11::as_sexp(colors);
}
