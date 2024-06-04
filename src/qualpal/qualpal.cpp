#include "qualpal.h"
#include "color_grid.h"
#include "cvd_simulation.h"
#include "farthest_points.h"
#include <cassert>
#include <map>
#include <regex>
#include <vector>

namespace qualpal {

// Check if the input is a valid HTML hex color representation
bool
isValidHexColor(const std::string& color)
{
  std::regex color_regex("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$");
  return std::regex_match(color, color_regex);
}

std::vector<RGB>
qualpal(const int n,
        std::vector<RGB> rgb_colors,
        const std::map<std::string, double>& cvd)
{
  int N = rgb_colors.size();

  const std::vector<RGB> rgb_colors_original = rgb_colors;

  for (const auto& [cvd_type, cvd_severity] : cvd) {
    if (cvd_severity > 0) {
      rgb_colors = simulate_cvd(rgb_colors, cvd_type, cvd_severity);
    }
  }

  std::vector<DIN99d> din99d_colors;
  din99d_colors.reserve(N);

  for (const auto& rgb : rgb_colors) {
    din99d_colors.emplace_back(rgb);
  }

  auto ind = farthestPoints(n, din99d_colors);

  std::vector<RGB> rgb_out;
  rgb_out.reserve(n);

  for (const auto& i : ind) {
    rgb_out.emplace_back(rgb_colors_original[i]);
  }

  return rgb_out;
}

std::vector<RGB>
qualpal(const int n,
        const std::vector<std::string>& hex_colors,
        const std::map<std::string, double>& cvd)
{
  std::vector<qualpal::RGB> rgb_colors;

  for (auto color : hex_colors) {
    assert(isValidHexColor(color) && "Values are hex colors");
    rgb_colors.emplace_back(color);
  }

  return qualpal(n, rgb_colors, cvd);
}

std::vector<RGB>
qualpal(const int n,
        const std::array<double, 2>& h_lim,
        const std::array<double, 2>& s_lim,
        const std::array<double, 2>& l_lim,
        const int n_points,
        const std::map<std::string, double>& cvd)
{
  if (n > n_points) {
    throw std::invalid_argument("Number of colors to generate must be lower "
                                "than the number of points in the color grid");
  }

  if (h_lim[0] < 0 || h_lim[1] > 360) {
    throw std::invalid_argument("Hue must be between 0 and 360");
  }

  if (s_lim[0] < 0 || s_lim[1] > 1) {
    throw std::invalid_argument("Saturation must be between 0 and 1");
  }

  if (l_lim[0] < 0 || l_lim[1] > 1) {
    throw std::invalid_argument("Lightness must be between 0 and 1");
  }

  auto hsl_colors = colorGrid(h_lim, s_lim, l_lim, n_points);

  std::vector<qualpal::RGB> rgb_colors;

  for (auto color : hsl_colors) {
    rgb_colors.emplace_back(color);
  }

  return qualpal(n, rgb_colors, cvd);
}

} // namespace qualpal
