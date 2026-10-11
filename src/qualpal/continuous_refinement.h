#pragma once

#include <array>
#include <map>
#include <qualpal/colors.h>
#include <qualpal/qualpal.h>
#include <string>
#include <vector>

namespace qualpal {

// Continuous local refinement that runs after the discrete farthestPoints
// swap. Each movable point is shifted in CIE L*a*b* to maximize its minimum
// CIEDE2000 distance to all other selected points (under normal vision and
// any active CVD simulation). Strict-improvement is required, so the same
// monotonicity argument as the swap loop guarantees no cycles.
//
// `selected` layout:
//   [0, n_fixed)                          fixed colors, never moved
//   [n_fixed, selected.size() - has_bg)   movable colors
//   selected.back() (when has_bg)         background, never moved
//
// When `regions` is non-empty, each candidate move must also lie inside the
// user's input region (HSL or LCHab) — otherwise refinement could push colors
// outside the region the candidate sampler was constrained to.
//
// Returns the refined XYZ vector plus a `moved` mask: callers should replace
// the original RGB only for moved entries, since the XYZ→RGB roundtrip on
// unchanged out-of-gamut colors is not always the identity.
struct RefinementResult
{
  std::vector<colors::XYZ> selected;
  std::vector<bool> moved;
};

RefinementResult
refinePalette(std::vector<colors::XYZ> selected,
              std::size_t n_fixed,
              bool has_bg,
              const std::vector<ColorspaceRegion>& regions,
              ColorspaceType space,
              const std::array<double, 3>& white_point,
              const std::map<std::string, double>& cvd);

} // namespace qualpal
