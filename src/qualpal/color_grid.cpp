#include "color_grid.h"
#include <array>
#include <cassert>
#include <cmath>

namespace qualpal {

std::vector<double>
linspace(const std::array<double, 2>& range, const int n)
{
  double min = range[0];
  double max = range[1];

  assert(min <= max && "min must be less than or equal to max");

  std::vector<double> result;
  result.reserve(n);

  const double step = (max - min) / static_cast<double>(n - 1);

  for (int i = 0; i < n; i++) {
    result.emplace_back(min + i * step);
  }

  return result;
}

std::vector<HSL>
colorGrid(const std::array<double, 2>& h_lim,
          const std::array<double, 2>& s_lim,
          const std::array<double, 2>& l_lim,
          const int n)
{
  std::vector<HSL> colors;
  colors.reserve(n);

  int n_i = std::round(std::cbrt(n));

  assert(n_i > 0 && "n_h must be larger than zero");

  std::vector<double> h_vec = linspace(h_lim, n_i);
  std::vector<double> s_vec = linspace(s_lim, n_i);
  std::vector<double> l_vec = linspace(l_lim, n_i);

  for (const auto& h : h_vec) {
    for (const auto& s : s_vec) {
      for (const auto& l : l_vec) {
        colors.emplace_back(h, s, l);
      }
    }
  }

  colors.shrink_to_fit();

  return colors;
}

} // namespace qualpal
