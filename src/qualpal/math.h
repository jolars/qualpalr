#pragma once

#include <cmath>

namespace qualpal {

inline double
mod(double a, double b)
{
  double rem = std::remainder(a, b);
  if (rem < 0)
    rem += b;
  return rem;
}

} // namespace qualpal
