#include "color_grid.h"
#include "cvd.h"
#include "farthest_points.h"
#include "palettes.h"
#include "palettes_data.h"
#include "validation.h"
#include <cassert>
#include <qualpal/colors.h>
#include <qualpal/qualpal.h>
#include <stdexcept>

namespace qualpal {

Qualpal&
Qualpal::setInputRGB(const std::vector<colors::RGB>& colors)
{
  this->rgb_colors = colors;
  this->mode = Mode::RGB;
  return *this;
}

Qualpal&
Qualpal::setInputHex(const std::vector<std::string>& hex_colors)
{
  for (const auto& color : hex_colors) {
    if (!isValidHexColor(color)) {
      throw std::invalid_argument("Invalid hex color: " + color +
                                  ". Expected format: #RRGGBB or #RGB");
    }
  }
  this->hex_colors = hex_colors;
  this->mode = Mode::HEX;
  return *this;
}

Qualpal&
Qualpal::setInputPalette(const std::string& palette)
{
  validatePalette(palette);
  this->palette = palette;
  this->mode = Mode::PALETTE;
  return *this;
}

Qualpal&
Qualpal::setInputColorspace(const std::array<double, 2>& h_lim,
                            const std::array<double, 2>& s_or_c_lim,
                            const std::array<double, 2>& l_lim,
                            ColorspaceType space)
{
  if (space == ColorspaceType::HSL) {
    if (h_lim[0] < -360 || h_lim[1] > 360) {
      throw std::invalid_argument("Hue must be between -360 and 360");
    }

    if (h_lim[1] - h_lim[0] > 360) {
      throw std::invalid_argument("Hue range must be less than 360");
    }

    if (s_or_c_lim[0] < 0 || s_or_c_lim[1] > 1) {
      throw std::invalid_argument("Saturation/chroma must be between 0 and 1");
    }

    if (l_lim[0] < 0 || l_lim[1] > 1) {
      throw std::invalid_argument("Lightness must be between 0 and 1");
    }
  } else if (space == ColorspaceType::LCHab) {
    if (h_lim[0] < 0 || h_lim[1] > 360) {
      throw std::invalid_argument("Hue must be between 0 and 360");
    }

    if (s_or_c_lim[0] < 0) {
      throw std::invalid_argument("Chroma must be non-negative");
    }

    if (l_lim[0] < 0 || l_lim[1] > 100) {
      throw std::invalid_argument("Lightness must be between 0 and 100");
    }
  }

  this->h_lim = h_lim;
  this->s_or_c_lim = s_or_c_lim;
  this->l_lim = l_lim;
  this->colorspace_input = space;
  this->mode = Mode::COLORSPACE;
  return *this;
}

Qualpal&
Qualpal::setCvd(const std::map<std::string, double>& cvd_params)
{
  this->cvd = cvd_params;
  return *this;
}

Qualpal&
Qualpal::setBackground(const colors::RGB& bg_color)
{
  this->bg = bg_color;
  return *this;
}

Qualpal&
Qualpal::setMetric(metrics::MetricType metric)
{
  this->metric = metric;
  return *this;
}

Qualpal&
Qualpal::setMemoryLimit(double gb)
{
  if (gb <= 0) {
    throw std::invalid_argument("Memory limit must be greater than 0");
  }
  this->max_memory = gb;
  return *this;
}

Qualpal&
Qualpal::setColorspaceSize(int n_points)
{
  if (n_points <= 0) {
    throw std::invalid_argument("Number of points must be greater than 0");
  }
  this->n_points = n_points;
  return *this;
}

std::vector<colors::RGB>
Qualpal::selectColors(int n, const std::vector<colors::RGB>& fixed_palette)
{
  switch (mode) {
    case Mode::RGB:
      break;
    case Mode::HEX:
      rgb_colors.clear();
      rgb_colors.reserve(n_points);
      for (const auto& hex : hex_colors) {
        rgb_colors.emplace_back(hex);
      }
      break;
    case Mode::PALETTE:
      rgb_colors.clear();
      rgb_colors.reserve(n_points);
      for (const auto& hex : getPalette(palette)) {
        rgb_colors.emplace_back(hex);
      }
      break;
    case Mode::COLORSPACE:
      rgb_colors.clear();
      rgb_colors.reserve(n_points);
      if (colorspace_input == ColorspaceType::HSL) {
        for (const auto& hsl :
             colorGrid<colors::HSL>(h_lim, s_or_c_lim, l_lim, n_points)) {
          rgb_colors.emplace_back(hsl);
        }
      } else if (colorspace_input == ColorspaceType::LCHab) {
        for (const auto& hsl :
             colorGrid<colors::LCHab>(h_lim, s_or_c_lim, l_lim, n_points)) {
          rgb_colors.emplace_back(hsl);
        }
      }
      break;
    default:
      throw std::runtime_error("No input source configured.");
  }

  if (rgb_colors.empty()) {
    throw std::runtime_error("No input colors provided.");
  }

  int n_fixed = fixed_palette.size();

  if (n < n_fixed) {
    throw std::invalid_argument(
      "Requested palette size is less than the size of the existing palette.");
  }

  int n_new = n - n_fixed;

  if (n_new < 0) {
    throw std::invalid_argument("Number of new colors to add is negative.");
  }

  if (static_cast<int>(rgb_colors.size()) < n_new) {
    throw std::invalid_argument(
      "Requested number of colors exceeds input size");
  }

  // Simulate CVD if needed
  std::vector<colors::RGB> rgb_colors_mod = rgb_colors;
  std::vector<colors::RGB> fixed_mod = fixed_palette;
  std::optional<colors::RGB> bg_mod = bg;

  for (const auto& [cvd_type, cvd_severity] : cvd) {
    if (cvd_severity > 1.0 || cvd_severity < 0.0) {
      throw std::invalid_argument("cvd_severity must be between 0 and 1");
    }
    if (cvd_severity > 0) {
      for (auto& rgb : rgb_colors_mod) {
        rgb = simulateCvd(rgb, cvd_type, cvd_severity);
      }
      for (auto& rgb : fixed_mod) {
        rgb = simulateCvd(rgb, cvd_type, cvd_severity);
      }
      if (bg_mod.has_value()) {
        bg_mod = simulateCvd(*bg_mod, cvd_type, cvd_severity);
      }
    }
  }

  std::vector<colors::XYZ> xyz_colors;
  for (const auto& c : rgb_colors_mod) {
    xyz_colors.emplace_back(c);
  }

  std::vector<colors::XYZ> xyz_fixed;
  for (const auto& c : fixed_mod) {
    xyz_fixed.emplace_back(c);
  }

  // Select new colors
  auto ind =
    farthestPoints(n, xyz_colors, metric, bg_mod, xyz_fixed, max_memory);

  // Output: fixed_palette + selected new colors
  std::vector<colors::RGB> result = fixed_palette;
  for (const auto& i : ind) {
    result.push_back(rgb_colors[i]);
  }

  return result;
}

std::vector<colors::RGB>
Qualpal::generate(int n)
{
  return selectColors(n);
}

std::vector<colors::RGB>
Qualpal::extend(const std::vector<colors::RGB>& palette, int n)
{
  return selectColors(n, palette);
}

} // namespace qualpal
