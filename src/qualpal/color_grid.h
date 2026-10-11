#include <algorithm>
#include <array>
#include <qualpal/colors.h>
#include <vector>

namespace qualpal {

template<std::size_t Dims>
class Halton
{
public:
  Halton()
  {
    static_assert(Dims <= 25, "Dimensions of more than 25 are not supported");
  }

  std::array<double, Dims> next()
  {
    std::array<double, Dims> result;

    for (std::size_t i = 0; i < Dims; i++) {
      result[i] = halton(counter, primes[i]);
    }

    ++counter;

    return result;
  }

private:
  double halton(const int index, const int base)
  {
    double f = 1.0;
    double result = 0;

    int i = index;

    while (i > 0) {
      f = f / base;
      result = result + f * (i % base);
      i = i / base;
    }

    return result;
  }

  unsigned int counter = 0;
  const std::array<int, 25> primes = { 2,  3,  5,  7,  11, 13, 17, 19, 23,
                                       29, 31, 37, 41, 43, 47, 53, 59, 61,
                                       67, 71, 73, 79, 83, 89, 97 };
};

// Scale x from [0, 1] to [min, max]
inline double
scaleToInterval(const double x, const double min, const double max)
{
  double x_scaled = (max - min) * (x - 1) + max;
  return std::clamp(x_scaled, min, max);
}

template<typename ColorType>
std::vector<ColorType>
colorGrid(const std::array<double, 2>& angle_lim,
          const std::array<double, 2>& radius_lim,
          const std::array<double, 2>& height_lim,
          std::size_t n_points)
{
  std::vector<ColorType> colors;
  colors.reserve(n_points);
  Halton<3> halton;

  for (std::size_t i = 0; i < n_points; ++i) {
    auto vals = halton.next();
    double phi = scaleToInterval(vals[0], angle_lim[0], angle_lim[1]);
    double r = scaleToInterval(vals[1], radius_lim[0], radius_lim[1]);
    double h = scaleToInterval(vals[2], height_lim[0], height_lim[1]);
    colors.emplace_back(phi < 0 ? phi + 360 : phi, r, h);
  }

  return colors;
}

// HSL: (hue, saturation, lightness)
template<>
inline std::vector<colors::HSL>
colorGrid(const std::array<double, 2>& hue_lim,
          const std::array<double, 2>& sat_lim,
          const std::array<double, 2>& light_lim,
          std::size_t n_points)
{
  std::vector<colors::HSL> colors;
  colors.reserve(n_points);
  Halton<3> halton;
  for (std::size_t i = 0; i < n_points; ++i) {
    auto vals = halton.next();
    double h = scaleToInterval(vals[0], hue_lim[0], hue_lim[1]);
    double s = scaleToInterval(vals[1], sat_lim[0], sat_lim[1]);
    double l = scaleToInterval(vals[2], light_lim[0], light_lim[1]);
    colors.emplace_back(h < 0 ? h + 360 : h, s, l);
  }
  return colors;
}

// LCHab: (lightness, chroma, hue)
template<>
inline std::vector<colors::LCHab>
colorGrid(const std::array<double, 2>& hue_lim,
          const std::array<double, 2>& chroma_lim,
          const std::array<double, 2>& light_lim,
          std::size_t n_points)
{
  std::vector<colors::LCHab> colors;
  colors.reserve(n_points);
  Halton<3> halton;
  for (std::size_t i = 0; i < n_points; ++i) {
    auto vals = halton.next();
    double h = scaleToInterval(vals[0], hue_lim[0], hue_lim[1]);
    double c = scaleToInterval(vals[1], chroma_lim[0], chroma_lim[1]);
    double l = scaleToInterval(vals[2], light_lim[0], light_lim[1]);
    colors.emplace_back(l, c, h < 0 ? h + 360 : h);
  }
  return colors;
}

// Direct HSL-Halton sampling; used as a fallback when an HSL axis is
// narrow enough that LCHab-bounding-box rejection sampling would have a
// catastrophic accept rate. The HSL→LCHab map is nonlinear, so a pinned HSL
// axis does NOT correspond to a pinned LCHab axis — pinning is best
// preserved by sampling in HSL coordinates directly.
inline std::vector<colors::RGB>
hslColorGridDirect(const std::array<double, 2>& h_lim_hsl,
                   const std::array<double, 2>& s_lim_hsl,
                   const std::array<double, 2>& l_lim_hsl,
                   std::size_t n_points)
{
  auto hsl = colorGrid<colors::HSL>(h_lim_hsl, s_lim_hsl, l_lim_hsl, n_points);
  std::vector<colors::RGB> out;
  out.reserve(hsl.size());
  for (const auto& c : hsl) {
    out.emplace_back(c);
  }
  return out;
}

// Halton-sample inside an LCHab box that covers sRGB, reject to user HSL
// bounds, and stop after a fixed attempt budget. Returns however many
// candidates were collected (may be fewer than n_points). Used as the
// perceptual-uniformity stage of hslColorGridViaLch.
inline std::vector<colors::RGB>
hslColorGridLchAttempt(const std::array<double, 2>& h_lim_hsl,
                       const std::array<double, 2>& s_lim_hsl,
                       const std::array<double, 2>& l_lim_hsl,
                       std::size_t n_points,
                       std::size_t max_attempts)
{
  // LCHab box generous enough to cover sRGB.
  constexpr double L_LO = 0.0, L_HI = 100.0;
  constexpr double C_LO = 0.0, C_HI = 150.0;

  Halton<3> halton;
  std::vector<colors::RGB> out;
  out.reserve(n_points);

  std::size_t attempts = 0;
  while (out.size() < n_points && attempts < max_attempts) {
    auto v = halton.next();
    ++attempts;

    double L = L_LO + v[0] * (L_HI - L_LO);
    double C = C_LO + v[1] * (C_HI - C_LO);
    double h_lch = v[2] * 360.0;

    colors::LCHab lch(L, C, h_lch);
    colors::RGB rgb(lch);

    if (rgb.r() < 0.0 || rgb.r() > 1.0 || rgb.g() < 0.0 || rgb.g() > 1.0 ||
        rgb.b() < 0.0 || rgb.b() > 1.0) {
      continue;
    }

    colors::HSL hsl(rgb);

    if (hsl.s() < s_lim_hsl[0] || hsl.s() > s_lim_hsl[1]) {
      continue;
    }
    if (hsl.l() < l_lim_hsl[0] || hsl.l() > l_lim_hsl[1]) {
      continue;
    }

    // Hue range can be specified in [-360, 360] with wrap; check three
    // offsets to cover crossings of 0/360.
    double h = hsl.h();
    bool in_h = false;
    for (double offset : { -360.0, 0.0, 360.0 }) {
      double hh = h + offset;
      if (hh >= h_lim_hsl[0] && hh <= h_lim_hsl[1]) {
        in_h = true;
        break;
      }
    }
    if (!in_h) {
      continue;
    }

    out.push_back(rgb);
  }

  return out;
}

// Generate HSL-constrained candidates with the most-uniform-in-Lab strategy
// achievable inside a fixed compute budget: try LCHab-uniform sampling with
// HSL-bounds rejection up to a cap, then top up any shortfall with direct
// HSL-Halton. The cap creates a soft transition — wide HSL ranges fill from
// LCHab before the cap hits, narrow/pinned HSL ranges blow through the cap
// and fill the remainder from HSL where the constraint is exact.
inline std::vector<colors::RGB>
hslColorGridViaLch(const std::array<double, 2>& h_lim_hsl,
                   const std::array<double, 2>& s_lim_hsl,
                   const std::array<double, 2>& l_lim_hsl,
                   std::size_t n_points)
{
  // Budget: 50 attempts per output. Wide HSL ranges (>10% LCHab acceptance)
  // finish well below the cap; ranges that drive acceptance under ~2% start
  // requiring HSL top-up.
  const std::size_t max_attempts = std::max<std::size_t>(n_points * 50, 20000);

  auto out = hslColorGridLchAttempt(
    h_lim_hsl, s_lim_hsl, l_lim_hsl, n_points, max_attempts);

  if (out.size() < n_points) {
    std::size_t deficit = n_points - out.size();
    auto top_up = hslColorGridDirect(h_lim_hsl, s_lim_hsl, l_lim_hsl, deficit);
    out.insert(out.end(), top_up.begin(), top_up.end());
  }

  return out;
}

} // namespace qualpal
