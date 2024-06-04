#include "distance_matrix.h"
#include <cmath>
#include <vector>

namespace qualpal {

Matrix<double>
distanceMatrix(const std::vector<DIN99d>& x)
{
  const int N = x.size();
  Matrix<double> result(N, N);

  for (int i = 0; i < N; ++i) {
    for (int j = i; j < N; ++j) {
      double d = std::sqrt(std::pow(x[i].l() - x[j].l(), 2) +
                           std::pow(x[i].a() - x[j].a(), 2) +
                           std::pow(x[i].b() - x[j].b(), 2));
      d = std::pow(d, 0.74) * 1.28;
      result(i, j) = d;
      result(j, i) = d;
    }
  }

  return result;
}
} // namespace qualpal
