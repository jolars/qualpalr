#include "color_grid.h"
#include "continuous_refinement.h"
#include "cvd.h"
#include "farthest_points.h"
#include "palettes.h"
#include "palettes_data.h"
#include "validation.h"
#include <cassert>
#include <limits>
#include <qualpal/color_difference.h>
#include <qualpal/colors.h>
#include <qualpal/qualpal.h>
#include <qualpal/threads.h>
#include <random>
#include <stdexcept>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace qualpal {

Qualpal&
Qualpal::setInputRGB(const std::vector<colors::RGB>& colors)
{
  this->rgb_colors_in = colors;
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
  ColorspaceRegion region{ h_lim, s_or_c_lim, l_lim };
  return setInputColorspaceRegions({ region }, space);
}

Qualpal&
Qualpal::setInputColorspaceRegions(const std::vector<ColorspaceRegion>& regions,
                                   ColorspaceType space)
{
  if (regions.empty()) {
    throw std::invalid_argument("At least one colorspace region is required");
  }

  // Validate each region
  for (const auto& region : regions) {
    if (space == ColorspaceType::HSL) {
      if (region.h_lim[0] < -360 || region.h_lim[1] > 360) {
        throw std::invalid_argument("Hue must be between -360 and 360");
      }

      if (region.h_lim[1] - region.h_lim[0] > 360) {
        throw std::invalid_argument("Hue range must be less than 360");
      }

      if (region.s_or_c_lim[0] < 0 || region.s_or_c_lim[1] > 1) {
        throw std::invalid_argument(
          "Saturation/chroma must be between 0 and 1");
      }

      if (region.l_lim[0] < 0 || region.l_lim[1] > 1) {
        throw std::invalid_argument("Lightness must be between 0 and 1");
      }
    } else if (space == ColorspaceType::LCHab) {
      if (region.h_lim[0] < -360 || region.h_lim[1] > 360) {
        throw std::invalid_argument("Hue must be between -360 and 360");
      }

      if (region.s_or_c_lim[0] < 0) {
        throw std::invalid_argument("Chroma must be non-negative");
      }

      if (region.l_lim[0] < 0 || region.l_lim[1] > 100) {
        throw std::invalid_argument("Lightness must be between 0 and 100");
      }
    }
  }

  this->colorspace_regions = regions;
  this->colorspace_input = space;
  this->mode = Mode::COLORSPACE;
  return *this;
}

Qualpal&
Qualpal::setCvd(const std::map<std::string, double>& cvd_params)
{
  for (const auto& [cvd_type, cvd_severity] : cvd_params) {
    if (cvd_severity > 1.0 || cvd_severity < 0.0) {
      throw std::invalid_argument("cvd_severity must be between 0 and 1");
    }
    if (cvd_type != "protan" && cvd_type != "deutan" && cvd_type != "tritan") {
      throw std::invalid_argument(
        "Invalid CVD type: " + cvd_type +
        ". Supported types are: protan, deutan, tritan.");
    }
  }
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
Qualpal::setColorspaceSize(std::size_t n_points)
{
  if (n_points <= 0) {
    throw std::invalid_argument("Number of points must be greater than 0");
  }
  this->n_points = n_points;
  return *this;
}

Qualpal&
Qualpal::setWhitePoint(WhitePoint wp)
{
  this->white_point = whitePointToXYZ(wp);
  return *this;
}

Qualpal&
Qualpal::setWhitePoint(const std::array<double, 3>& white_point)
{
  this->white_point = white_point;
  return *this;
}

Qualpal&
Qualpal::setRefinementStarts(int n_starts)
{
  if (n_starts < 0) {
    throw std::invalid_argument("Number of refinement starts must be >= 0");
  }
  this->n_refinement_starts = n_starts;
  return *this;
}

namespace {

bool
inSrgbGamutLinear(const colors::Lab& lab, const std::array<double, 3>& wp)
{
  colors::XYZ xyz(lab, wp);
  const double x = xyz.x();
  const double y = xyz.y();
  const double z = xyz.z();
  const double r = 3.2404542 * x - 1.5371385 * y - 0.4985314 * z;
  const double g = -0.9692660 * x + 1.8760108 * y + 0.0415560 * z;
  const double b = 0.0556434 * x - 0.2040259 * y + 1.0572252 * z;
  constexpr double eps = 1e-9;
  return r >= -eps && r <= 1.0 + eps && g >= -eps && g <= 1.0 + eps &&
         b >= -eps && b <= 1.0 + eps;
}

bool
labInRegion(const colors::Lab& lab,
            const ColorspaceRegion& r,
            ColorspaceType space)
{
  double h, axis2, axis3;
  if (space == ColorspaceType::HSL) {
    colors::HSL hsl(lab);
    h = hsl.h();
    axis2 = hsl.s();
    axis3 = hsl.l();
  } else {
    colors::LCHab lch(lab);
    h = lch.h();
    axis2 = lch.c();
    axis3 = lch.l();
  }
  if (axis2 < r.s_or_c_lim[0] || axis2 > r.s_or_c_lim[1])
    return false;
  if (axis3 < r.l_lim[0] || axis3 > r.l_lim[1])
    return false;
  for (double off : { -360.0, 0.0, 360.0 }) {
    double hh = h + off;
    if (hh >= r.h_lim[0] && hh <= r.h_lim[1])
      return true;
  }
  return false;
}

// Sample one in-region, in-gamut Lab point from the union of regions.
// Falls back to a clamped random Lab on persistent rejection so the seed
// loop always terminates.
colors::Lab
sampleRandomLab(std::mt19937& rng,
                const std::vector<ColorspaceRegion>& regions,
                ColorspaceType space,
                const std::array<double, 3>& wp)
{
  std::uniform_int_distribution<size_t> region_pick(0, regions.size() - 1);
  for (int attempt = 0; attempt < 10000; ++attempt) {
    const auto& r = regions[region_pick(rng)];
    std::uniform_real_distribution<double> hd(r.h_lim[0], r.h_lim[1]);
    std::uniform_real_distribution<double> sd(r.s_or_c_lim[0],
                                              r.s_or_c_lim[1]);
    std::uniform_real_distribution<double> ld(r.l_lim[0], r.l_lim[1]);
    double h = hd(rng);
    if (h < 0)
      h += 360;
    if (h >= 360)
      h -= 360;
    colors::Lab lab = space == ColorspaceType::HSL
                        ? colors::Lab(colors::HSL(h, sd(rng), ld(rng)))
                        : colors::Lab(colors::LCHab(ld(rng), sd(rng), h));
    if (!inSrgbGamutLinear(lab, wp))
      continue;
    if (!labInRegion(lab, r, space))
      continue;
    return lab;
  }
  // Pathological case (all-degenerate regions); return a centered fallback.
  const auto& r = regions.front();
  double l_mid = 0.5 * (r.l_lim[0] + r.l_lim[1]);
  double s_mid = 0.5 * (r.s_or_c_lim[0] + r.s_or_c_lim[1]);
  double h_mid = 0.5 * (r.h_lim[0] + r.h_lim[1]);
  if (h_mid < 0)
    h_mid += 360;
  return space == ColorspaceType::HSL
           ? colors::Lab(colors::HSL(h_mid, s_mid, l_mid))
           : colors::Lab(colors::LCHab(l_mid, s_mid, h_mid));
}

// Score a palette by the worst-case CIEDE2000 distance between any two
// points (and against the background, when present), taking the minimum
// across normal vision and each active CVD simulation.
double
scorePalette(const std::vector<colors::RGB>& pal,
             const std::optional<colors::RGB>& bg,
             const std::map<std::string, double>& cvd)
{
  metrics::CIEDE2000 dE;
  std::vector<colors::RGB> all = pal;
  if (bg)
    all.push_back(*bg);
  std::vector<std::vector<colors::Lab>> views(all.size());
  for (size_t i = 0; i < all.size(); ++i) {
    views[i].emplace_back(all[i]);
    for (const auto& [type, sev] : cvd) {
      if (sev > 0.0)
        views[i].emplace_back(simulateCvd(all[i], type, sev));
    }
  }
  double m = std::numeric_limits<double>::infinity();
  for (size_t i = 0; i < all.size(); ++i) {
    for (size_t j = i + 1; j < all.size(); ++j) {
      double d = std::numeric_limits<double>::max();
      for (size_t k = 0; k < views[i].size(); ++k) {
        d = std::min(d, dE(views[i][k], views[j][k]));
      }
      m = std::min(m, d);
    }
  }
  return m;
}

} // namespace

std::vector<colors::RGB>
Qualpal::selectColors(std::size_t n,
                      const std::vector<colors::RGB>& fixed_palette)
{
  switch (mode) {
    case Mode::RGB:
      break;
    case Mode::HEX:
      rgb_colors_in.clear();
      rgb_colors_in.reserve(n_points);
      for (const auto& hex : hex_colors) {
        rgb_colors_in.emplace_back(hex);
      }
      break;
    case Mode::PALETTE:
      rgb_colors_in.clear();
      rgb_colors_in.reserve(n_points);
      for (const auto& hex : getPalette(palette)) {
        rgb_colors_in.emplace_back(hex);
      }
      break;
    case Mode::COLORSPACE: {
      rgb_colors_in.clear();

      // Calculate points per region (distribute evenly)
      std::size_t points_per_region = n_points / colorspace_regions.size();
      std::size_t remainder = n_points % colorspace_regions.size();

      rgb_colors_in.reserve(n_points);

      for (std::size_t i = 0; i < colorspace_regions.size(); ++i) {
        const auto& region = colorspace_regions[i];
        // Add one extra point to first 'remainder' regions to distribute evenly
        std::size_t region_points = points_per_region + (i < remainder ? 1 : 0);

        if (colorspace_input == ColorspaceType::HSL) {
          for (const auto& rgb : hslColorGridViaLch(region.h_lim,
                                                   region.s_or_c_lim,
                                                   region.l_lim,
                                                   region_points)) {
            rgb_colors_in.push_back(rgb);
          }
        } else if (colorspace_input == ColorspaceType::LCHab) {
          for (const auto& lch : colorGrid<colors::LCHab>(region.h_lim,
                                                          region.s_or_c_lim,
                                                          region.l_lim,
                                                          region_points)) {
            rgb_colors_in.emplace_back(lch);
          }
        }
      }
      break;
    }
    case Mode::NONE:
      throw std::runtime_error("No input source configured.");
  }

  if (rgb_colors_in.empty()) {
    throw std::runtime_error("No input colors provided.");
  }

  std::size_t n_fixed = fixed_palette.size();

  if (n < n_fixed) {
    throw std::invalid_argument(
      "Requested palette size is less than the size of the existing palette.");
  }

  if (n < n_fixed) {
    throw std::invalid_argument("Number of new colors to add is negative.");
  }

  std::size_t n_new = n - n_fixed;

  if (rgb_colors_in.size() < n_new) {
    throw std::invalid_argument(
      "Requested number of colors exceeds input size");
  }

  bool has_bg = bg.has_value();

  std::vector<colors::RGB> rgb_colors;
  rgb_colors.reserve(fixed_palette.size() + rgb_colors_in.size() +
                     (has_bg ? 1 : 0));
  rgb_colors.insert(
    rgb_colors.end(), fixed_palette.begin(), fixed_palette.end());
  rgb_colors.insert(
    rgb_colors.end(), rgb_colors_in.begin(), rgb_colors_in.end());
  if (has_bg) {
    rgb_colors.push_back(*bg);
  }

  // Convert colors to XYZ for distance calculations
  std::vector<colors::XYZ> xyz_colors;
  xyz_colors.reserve(rgb_colors.size());
  for (const auto& c : rgb_colors) {
    xyz_colors.emplace_back(c);
  }

  // Select new colors (CVD-aware if CVD parameters are set)
  auto ind = farthestPoints(
    n, xyz_colors, metric, has_bg, n_fixed, max_memory, white_point, cvd);

  // Continuous refinement only runs when the input is a colorspace region:
  // refining off-grid for fixed input sets (RGB/hex/named palette) would
  // violate the contract that the output be drawn from those inputs.
  bool do_refine =
    n_refinement_starts > 0 && mode == Mode::COLORSPACE && n_fixed < n;

  if (do_refine) {
    const std::size_t n_total = n + (has_bg ? 1 : 0);

    // Seed 0: discrete warm start + refine. Preserve the original RGB for
    // points that didn't move (XYZ→RGB roundtrip drift on out-of-gamut
    // candidates is non-trivial).
    std::vector<colors::XYZ> seed0_xyz;
    seed0_xyz.reserve(n_total);
    for (std::size_t i = 0; i < n; ++i) {
      seed0_xyz.push_back(xyz_colors[ind[i]]);
    }
    if (has_bg) {
      seed0_xyz.push_back(xyz_colors.back());
    }
    auto refined0 = refinePalette(std::move(seed0_xyz),
                                  n_fixed,
                                  has_bg,
                                  colorspace_regions,
                                  colorspace_input,
                                  white_point,
                                  cvd);
    std::vector<colors::RGB> seed0_pal;
    seed0_pal.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
      if (refined0.moved[i]) {
        seed0_pal.emplace_back(refined0.selected[i]);
      } else {
        seed0_pal.emplace_back(rgb_colors[ind[i]]);
      }
    }

    if (n_refinement_starts == 1) {
      return seed0_pal;
    }

    // Multi-start: also run refinement from random in-region, in-gamut
    // seeds and keep the highest-scoring palette. The discrete warm start
    // above is reliable but basin-bound; random seeds let us escape.
    const int n_extra = n_refinement_starts - 1;
    std::vector<std::vector<colors::RGB>> palettes(n_extra);
    std::vector<double> scores(n_extra,
                               -std::numeric_limits<double>::infinity());

    // Reuse the fixed-palette XYZ prefix and the bg suffix verbatim across
    // every random seed; only the movable slice is resampled.
    std::vector<colors::XYZ> prefix_suffix(n_total);
    for (std::size_t i = 0; i < n_fixed; ++i) {
      prefix_suffix[i] = xyz_colors[i];
    }
    if (has_bg) {
      prefix_suffix[n_total - 1] = xyz_colors.back();
    }

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic) num_threads(Threads::get())
#endif
    for (int s = 0; s < n_extra; ++s) {
      std::mt19937 rng(static_cast<uint64_t>(s) + 1);
      std::vector<colors::XYZ> seed_xyz = prefix_suffix;
      for (std::size_t i = n_fixed; i < n; ++i) {
        colors::Lab lab = sampleRandomLab(
          rng, colorspace_regions, colorspace_input, white_point);
        seed_xyz[i] = colors::XYZ(lab, white_point);
      }
      auto refined_s = refinePalette(std::move(seed_xyz),
                                     n_fixed,
                                     has_bg,
                                     colorspace_regions,
                                     colorspace_input,
                                     white_point,
                                     cvd);
      std::vector<colors::RGB> pal;
      pal.reserve(n);
      for (std::size_t i = 0; i < n; ++i) {
        // Fixed colors must round-trip exactly — use their original RGB
        // rather than RGB(XYZ(rgb)), which would drift on the gamut edge.
        if (i < n_fixed) {
          pal.emplace_back(rgb_colors[i]);
        } else {
          pal.emplace_back(refined_s.selected[i]);
        }
      }
      double sc = scorePalette(pal, bg, cvd);
      palettes[s] = std::move(pal);
      scores[s] = sc;
    }

    double best_score = scorePalette(seed0_pal, bg, cvd);
    std::vector<colors::RGB>* best = &seed0_pal;
    for (int s = 0; s < n_extra; ++s) {
      if (scores[s] > best_score) {
        best_score = scores[s];
        best = &palettes[s];
      }
    }
    return *best;
  }

  // Output: fixed_palette + selected new colors
  std::vector<colors::RGB> result;
  for (const auto& i : ind) {
    result.emplace_back(rgb_colors[i]);
  }

  return result;
}

std::vector<colors::RGB>
Qualpal::generate(std::size_t n)
{
  return selectColors(n);
}

std::vector<colors::RGB>
Qualpal::extend(const std::vector<colors::RGB>& palette, std::size_t n)
{
  return selectColors(n, palette);
}

} // namespace qualpal
