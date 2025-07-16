#include "validation.h"
#include <array>
#include <regex>

namespace qualpal {

bool
isValidHexColor(const std::string& color)
{
  std::regex color_regex("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$");
  return std::regex_match(color, color_regex);
}

void
validateHslRanges(const std::array<double, 2>& h_lim,
                  const std::array<double, 2>& s_lim,
                  const std::array<double, 2>& l_lim)
{
  if (h_lim[0] >= h_lim[1]) {
    throw std::invalid_argument(
      "Hue range: minimum (" + std::to_string(h_lim[0]) +
      ") must be less than maximum (" + std::to_string(h_lim[1]) + ")");
  }
  if (s_lim[0] >= s_lim[1] || s_lim[0] < 0 || s_lim[1] > 1) {
    throw std::invalid_argument(
      "Saturation range must be between 0-1 with min < max");
  }
  if (l_lim[0] >= l_lim[1] || l_lim[0] < 0 || l_lim[1] > 1) {
    throw std::invalid_argument(
      "Lightness range must be between 0-1 with min < max");
  }
}

}
