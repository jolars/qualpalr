/**
 * @file
 * @brief Functions for generating color difference matrices.
 *
 * Provides functions to compute pairwise color differences between a set of
 * colors using various perceptual color difference metrics (e.g., DIN99d,
 * CIEDE2000, CIE76). Supports parallel computation (OpenMP) and memory
 * management for large matrices. Used for palette analysis, selection, and
 * color distinguishability evaluation.
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
 * @brief Generate a symmetric color difference matrix for a set of colors.
 *
 * Computes the pairwise perceptual color differences between all colors in the
 * input vector, using the specified color difference metric (e.g., DIN99d,
 * CIEDE2000, CIE76). The result is a symmetric matrix where element (i, j) is
 * the distance between colors[i] and colors[j].
 *
 * @tparam ColorType Any color class convertible to the required color space
 * (e.g., colors::RGB, colors::HSL, colors::XYZ, colors::Lab, colors::DIN99d).
 * @tparam Metric Color difference metric functor (defaults to metrics::DIN99d).
 * @param colors Vector of colors to compare.
 * @param metric Color difference metric to use (optional; default is DIN99d).
 * @param max_memory Maximum memory (in GB) allowed for the matrix
 * (default: 1.0).
 * @return Matrix<double> Symmetric matrix of pairwise color differences [size:
 * n x n].
 * @throws std::invalid_argument if fewer than one color is provided.
 * @throws std::runtime_error if the estimated matrix size exceeds max_memory.
 *
 * @see metrics::DIN99d, metrics::CIEDE2000, metrics::CIE76
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
 * @brief Generate a color difference matrix for XYZ colors with runtime metric
 * selection.
 *
 * Computes the pairwise color differences between all colors in the input
 * vector, using the specified color difference metric (selected at runtime via
 * MetricType). Internally converts XYZ colors to the appropriate color space
 * for the selected metric.
 *
 * @param colors Vector of colors::XYZ colors to compare.
 * @param metric_type Color difference metric to use (DIN99d, CIEDE2000, or
 * CIE76).
 * @param max_memory Maximum memory (in GB) allowed for the matrix
 * (default: 1.0).
 * @return Matrix<double> Symmetric matrix of pairwise color differences [size:
 * n x n].
 * @throws std::invalid_argument if the metric type is unsupported or input is
 * invalid.
 * @throws std::runtime_error if the estimated matrix size exceeds max_memory.
 *
 * @see metrics::MetricType
 */
Matrix<double>
colorDifferenceMatrix(const std::vector<colors::XYZ>& colors,
                      const metrics::MetricType& metric_type,
                      const double max_memory = 1);

} // namespace qualpal
