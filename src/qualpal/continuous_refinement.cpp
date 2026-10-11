#include "continuous_refinement.h"
#include "cvd.h"
#include <algorithm>
#include <limits>
#include <qualpal/metrics.h>

namespace qualpal {
namespace {

bool
hueInRange(double h, const std::array<double, 2>& h_lim)
{
  for (double offset : { -360.0, 0.0, 360.0 }) {
    double hh = h + offset;
    if (hh >= h_lim[0] && hh <= h_lim[1]) {
      return true;
    }
  }
  return false;
}

bool
inRegions(const colors::Lab& lab,
          const std::vector<ColorspaceRegion>& regions,
          ColorspaceType space)
{
  if (regions.empty()) {
    return true;
  }
  if (space == ColorspaceType::HSL) {
    colors::HSL hsl(lab);
    for (const auto& r : regions) {
      if (hsl.s() >= r.s_or_c_lim[0] && hsl.s() <= r.s_or_c_lim[1] &&
          hsl.l() >= r.l_lim[0] && hsl.l() <= r.l_lim[1] &&
          hueInRange(hsl.h(), r.h_lim)) {
        return true;
      }
    }
    return false;
  }
  colors::LCHab lch(lab);
  for (const auto& r : regions) {
    if (lch.c() >= r.s_or_c_lim[0] && lch.c() <= r.s_or_c_lim[1] &&
        lch.l() >= r.l_lim[0] && lch.l() <= r.l_lim[1] &&
        hueInRange(lch.h(), r.h_lim)) {
      return true;
    }
  }
  return false;
}

// RGB::RGB(XYZ) silently clamps to [0,1], so we cannot use it to detect
// out-of-gamut colors. Instead, compute the linear-sRGB matrix product
// directly: companding is monotonic on [0,1] and maps the unit interval to
// itself, so a color is in sRGB gamut iff every linear-RGB component is.
bool
inSrgbGamut(const colors::Lab& lab, const std::array<double, 3>& wp)
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

std::vector<colors::Lab>
makeViews(const colors::XYZ& xyz,
          const std::array<double, 3>& wp,
          const std::map<std::string, double>& cvd)
{
  std::vector<colors::Lab> views;
  views.reserve(1 + cvd.size());
  views.emplace_back(xyz, wp);
  if (!cvd.empty()) {
    colors::RGB rgb(xyz);
    for (const auto& [type, severity] : cvd) {
      if (severity > 0.0) {
        colors::RGB sim = simulateCvd(rgb, type, severity);
        views.emplace_back(colors::Lab(colors::XYZ(sim), wp));
      }
    }
  }
  return views;
}

double
minDistOverViews(const std::vector<colors::Lab>& a,
                 const std::vector<colors::Lab>& b)
{
  metrics::CIEDE2000 dE;
  double m = std::numeric_limits<double>::max();
  for (std::size_t k = 0; k < a.size(); ++k) {
    m = std::min(m, dE(a[k], b[k]));
  }
  return m;
}

} // namespace

RefinementResult
refinePalette(std::vector<colors::XYZ> selected,
              std::size_t n_fixed,
              bool has_bg,
              const std::vector<ColorspaceRegion>& regions,
              ColorspaceType space,
              const std::array<double, 3>& white_point,
              const std::map<std::string, double>& cvd)
{
  const std::size_t n_total = selected.size();
  const std::size_t movable_end = n_total - (has_bg ? 1 : 0);
  std::vector<bool> moved(n_total, false);
  if (n_fixed >= movable_end) {
    return { std::move(selected), std::move(moved) };
  }

  std::vector<std::vector<colors::Lab>> views(n_total);
  for (std::size_t i = 0; i < n_total; ++i) {
    views[i] = makeViews(selected[i], white_point, cvd);
  }

  // Coarse-to-fine grid: ΔE≈4 → ΔE≈1 → ΔE≈0.25, each a 7^3 cube around the
  // current best. Within a level, re-center on improvement and rescan.
  struct Level
  {
    double radius;
    int half;
  };
  const std::array<Level, 3> levels = {
    { { 4.0, 3 }, { 1.0, 3 }, { 0.25, 3 } }
  };

  bool any_changed = true;
  std::size_t pass = 0;
  const std::size_t max_passes = 8;

  while (any_changed && pass < max_passes) {
    any_changed = false;
    ++pass;

    for (std::size_t i = n_fixed; i < movable_end; ++i) {
      colors::Lab best_lab(selected[i], white_point);
      // The candidate sampler can yield colors whose RGB round-trip lands
      // outside the user's region; skip those rather than fight a no-win
      // strict-improvement search.
      if (!inRegions(best_lab, regions, space) ||
          !inSrgbGamut(best_lab, white_point)) {
        continue;
      }
      double current_min = std::numeric_limits<double>::max();
      for (std::size_t j = 0; j < n_total; ++j) {
        if (j == i)
          continue;
        current_min =
          std::min(current_min, minDistOverViews(views[i], views[j]));
      }

      double best_min = current_min;
      std::vector<colors::Lab> best_views = views[i];

      for (const auto& level : levels) {
        const double step = level.radius / level.half;
        bool level_changed = true;
        while (level_changed) {
          level_changed = false;
          colors::Lab center = best_lab;
          for (int di = -level.half; di <= level.half; ++di) {
            for (int dj = -level.half; dj <= level.half; ++dj) {
              for (int dk = -level.half; dk <= level.half; ++dk) {
                if (di == 0 && dj == 0 && dk == 0)
                  continue;
                double cand_l = center.l() + di * step;
                double cand_a = center.a() + dj * step;
                double cand_b = center.b() + dk * step;
                if (cand_l < 0.0 || cand_l > 100.0)
                  continue;
                if (cand_a < -128.0 || cand_a > 127.0)
                  continue;
                if (cand_b < -128.0 || cand_b > 127.0)
                  continue;
                colors::Lab cand(cand_l, cand_a, cand_b);
                if (!inSrgbGamut(cand, white_point))
                  continue;
                if (!inRegions(cand, regions, space))
                  continue;
                colors::XYZ cand_xyz(cand, white_point);
                auto cand_views = makeViews(cand_xyz, white_point, cvd);
                double m = std::numeric_limits<double>::max();
                for (std::size_t j = 0; j < n_total; ++j) {
                  if (j == i)
                    continue;
                  m = std::min(m, minDistOverViews(cand_views, views[j]));
                  if (m <= best_min)
                    break;
                }
                if (m > best_min) {
                  best_min = m;
                  best_lab = cand;
                  best_views = std::move(cand_views);
                  level_changed = true;
                }
              }
            }
          }
        }
      }

      if (best_min > current_min) {
        selected[i] = colors::XYZ(best_lab, white_point);
        views[i] = std::move(best_views);
        moved[i] = true;
        any_changed = true;
      }
    }
  }

  return { std::move(selected), std::move(moved) };
}

} // namespace qualpal
