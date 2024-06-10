#include "color_grid.h"
#include <array>
#include <cassert>
#include <cmath>
#include <vector>

namespace qualpal {

template<int Dims>
class Halton
{
public:
  Halton()
  {
    static_assert(Dims <= 25, "Dimensions of more than 25 are not supported");
  }

  std::array<double, Dims> next()
  {
    ++counter;
    std::array<double, Dims> result;

    for (int i = 0; i < Dims; i++) {
      result[i] = halton(counter, primes[i]);
    }

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

  unsigned int counter;
  const std::array<int, 25> primes = { 2,  3,  5,  7,  11, 13, 17, 19, 23,
                                       29, 31, 37, 41, 43, 47, 53, 59, 61,
                                       67, 71, 73, 79, 83, 89, 97 };
};

// Scale x from [0, 1] to [min, max]
double
scale_to_interval(const double x, const double min, const double max)
{
  return (max - min) * (x - 1) + max;
}

std::vector<HSL>
colorGrid(const std::array<double, 2>& h_lim,
          const std::array<double, 2>& s_lim,
          const std::array<double, 2>& l_lim,
          const int n)
{
  std::vector<HSL> colors;
  colors.reserve(n);

  Halton<3> halton;

  for (int i = 0; i < n; i++) {
    const std::array<double, 3> hsl = halton.next();

    double h = scale_to_interval(hsl[0], h_lim[0], h_lim[1]);
    double s = scale_to_interval(hsl[1], s_lim[0], s_lim[1]);
    double l = scale_to_interval(hsl[2], l_lim[0], l_lim[1]);

    // We allow negative hues to wrap around the color wheel, but here we
    // need to make sure that the hue is in the range [0, 360)
    colors.emplace_back(h < 0 ? h + 360 : h, s, l);
  }

  return colors;
}

} // namespace qualpal
