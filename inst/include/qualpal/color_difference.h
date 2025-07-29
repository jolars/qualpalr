/** @file
 * @brief Functions for generating color difference matrices.
 *
 * This file provides functions to compute pairwise color differences between
 * a set of colors using various color difference metrics. It supports parallel
 * computation and memory management.
 */

#pragma once

#include <cmath>
#include <qualpal/matrix.h>
#include <qualpal/metrics.h>
#include <qualpal/threads.h>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace qualpal {
namespace detail {

inline std::size_t
estimateMatrixMemory(std::size_t n)
{
  return n * n * sizeof(double);
}

inline bool
checkMatrixSize(std::size_t n, double max_gb = 1.0)
{
  double estimated_gb = estimateMatrixMemory(n) / (1024.0 * 1024.0 * 1024.0);
  return estimated_gb <= max_gb;
}
} // namespace detail

/**
 * @brief Generate a color difference matrix
 * @tparam ColorType Any color class (colors::RGB, colors::HSL, colors::XYZ,
 * colors::Lab, colors::DIN99d)
 * @tparam Metric Color difference metric (defaults to metrics::DIN99d)
 * @param colors Vector of colors to compare
 * @param metric Color difference metric to use
 * @param max_memory Maximum memory (in GB) allowed for the matrix
 * @return Symmetric matrix of pairwise color differences
 */
template<typename ColorType, typename Metric = metrics::DIN99d>
Matrix<double>
colorDifferenceMatrix(const std::vector<ColorType>& colors,
                      const Metric& metric = Metric{},
                      const double max_memory = 1)
{
  using namespace detail;

  const std::size_t n_colors = colors.size();

  if (n_colors < 1) {
    throw std::invalid_argument("At least one color is required to compute "
                                "a color difference matrix.");
  }

  if (!checkMatrixSize(n_colors, max_memory)) {
    throw std::runtime_error(
      "Color difference matrix would require " +
      std::to_string(estimateMatrixMemory(n_colors) /
                     (1024.0 * 1024.0 * 1024.0)) +
      " GB, which exceeds the limit of " + std::to_string(max_memory) +
      " GB. Reduce the number of colors or increase the memory limit.");
  }

  Matrix<double> result(n_colors, n_colors);

#ifdef _OPENMP
#pragma omp parallel for num_threads(Threads::get())
#endif
  for (std::size_t i = 0; i < n_colors; ++i) {
    result(i, i) = 0.0;
#ifdef _OPENMP
#pragma omp simd
#endif
    for (std::size_t j = i + 1; j < n_colors; ++j) {
      double d = metric(colors[i], colors[j]);
      result(i, j) = d;
      result(j, i) = d;
    }
  }
  return result;
}

/**
 * @brief Generate a color difference matrix for Lab colors with runtime metric
 * selection.
 *
 * @param colors Vector of XYZ colors to compare.
 * @param metric_type Color difference metric to use.
 * @param max_memory Maximum memory (in GB) allowed for the matrix.
 * @return Symmetric matrix of pairwise color differences.
 * @see metrics::MetricType
 */
Matrix<double>
colorDifferenceMatrix(const std::vector<colors::XYZ>& colors,
                      const metrics::MetricType& metric_type,
                      const double max_memory = 1);

} // namespace qualpal
