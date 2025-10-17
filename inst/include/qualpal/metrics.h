/**
 * @file
 * @brief Color difference metrics for qualpal.
 *
 * Provides classes and functions for computing perceptual color differences
 * using standard metrics (DIN99d, CIE76, CIEDE2000). These metrics are used
 * for palette analysis, selection, and color distinguishability evaluation.
 *
 * All metrics are callable objects (functors) and can be used with
 * colorDifferenceMatrix() and palette generation algorithms.
 */

#pragma once

#include <cassert>
#include <cmath>
#include <qualpal/colors.h>

namespace qualpal {

/**
 * @brief Color difference metrics for perceptual color comparison.
 *
 * Contains functor classes implementing standard color difference formulas
 * (DIN99d, CIE76, CIEDE2000) for quantifying perceptual differences between
 * colors. Used in palette selection (e.g., farthestPoints()), palette analysis,
 * and color distinguishability evaluation.
 *
 * All metric classes:
 * - Are callable with operator() taking two color objects.
 * - Are template-based to accept any color type convertible to the required
 * color space.
 * - Return a numeric difference value (Delta E; higher = more different).
 *
 * @see colorDifferenceMatrix() for computing full distance matrices.
 * @see farthestPoints() for palette selection using these metrics.
 */
namespace metrics {
namespace detail {
inline double
cosd(double degrees)
{
  return std::cos(degrees * M_PI / 180.0);
}

inline double
sind(double degrees)
{
  return std::sin(degrees * M_PI / 180.0);
}

inline double
tand(double degrees)
{
  return std::tan(degrees * M_PI / 180.0);
}

inline double
atan2d(double y, double x)
{
  double deg = std::atan2(y, x) * 180.0 / M_PI;
  return (deg >= 0) ? deg : deg + 360.0;
}

inline double
square(double x)
{
  return x * x;
}
} // namespace detail

// Forward declarations
class RGB;
class HSL;
class XYZ;
class Lab;
class DIN99d;

/**
 * @brief Supported color difference metrics for palette generation and
 * analysis.
 *
 * Use this enum to select the color difference formula for qualpal algorithms,
 * palette analysis, and CLI tools.
 *
 * - DIN99d: Perceptually uniform metric, robust for small differences.
 * - CIE76: Simple Euclidean distance in Lab space (Delta E 1976).
 * - CIEDE2000: State-of-the-art perceptual metric (Delta E 2000).
 */
enum class MetricType
{
  DIN99d,   ///< DIN99d color difference
  CIE76,    ///< CIE76 (Delta E 1976) color difference
  CIEDE2000 ///< CIEDE2000 (Delta E 2000) color difference
};

/**
 * @brief DIN99d color difference metric with optional power transformation.
 *
 * Computes the Euclidean distance between two colors in DIN99d color space.
 * Optionally applies a power transformation for improved perceptual uniformity.
 *
 * @tparam ColorType1, ColorType2 Any color type convertible to colors::DIN99d.
 *
 * @param use_power_transform Whether to apply power transformation (default:
 * true).
 * @param power Power value for transformation (default: 0.74).
 * @param scale Scale factor after transformation (default: 1.28).
 *
 * @see qualpal::colors::DIN99d
 */
class DIN99d
{
private:
  bool use_power_transform;
  double power;
  double scale;

public:
  /**
   * @brief Construct DIN99d metric with configurable parameters
   * @param use_power_transform Whether to apply power transformation (default:
   * true)
   * @param power Power value for transformation (default: 0.74)
   * @param scale Scale factor after transformation (default: 1.28)
   */
  explicit DIN99d(bool use_power_transform = true,
                  double power = 0.74,
                  double scale = 1.28)
    : use_power_transform(use_power_transform)
    , power(power)
    , scale(scale)
  {
  }

  /**
   * @brief Calculate color difference between two colors
   * @tparam ColorType1 Any color class that can be converted to colors::DIN99d
   * @tparam ColorType2 Any color class that can be converted to colors::DIN99d
   * @param c1 First color
   * @param c2 Second color
   * @return Color difference value (Delta E in DIN99d space)
   */
  template<typename ColorType1, typename ColorType2>
  double operator()(const ColorType1& c1, const ColorType2& c2) const
  {
    colors::DIN99d d1(c1), d2(c2);
    double d = std::hypot(d1.l() - d2.l(), d1.a() - d2.a(), d1.b() - d2.b());

    if (use_power_transform) {
      return std::pow(d, power) * scale;
    } else {
      return d;
    }
  }
};

/**
 * @brief CIE76 (Delta E 1976) color difference metric.
 *
 * Computes the Euclidean distance between two colors in CIE Lab space.
 * This is the simplest color difference formula, but less perceptually accurate
 * than DIN99d or CIEDE2000.
 *
 * @tparam ColorType1, ColorType2 Any color type convertible to colors::Lab.
 *
 * @see qualpal::colors::Lab
 */
struct CIE76
{
  /**
   * @brief Calculate CIE76 color difference
   * @tparam ColorType1 Any color class that can be converted to colors::DIN99d
   * @tparam ColorType2 Any color class that can be converted to colors::DIN99d
   * @param c1 First color
   * @param c2 Second color
   * @return CIE76 Delta E value
   */
  template<typename ColorType1, typename ColorType2>
  double operator()(const ColorType1& c1, const ColorType2& c2) const
  {
    colors::Lab l1(c1), l2(c2);
    return std::hypot(l1.l() - l2.l(), l1.a() - l2.a(), l1.b() - l2.b());
  }
};

/**
 * @brief CIEDE2000 (Delta E 2000) color difference metric.
 *
 * Computes the perceptual color difference between two colors using the
 * CIEDE2000 formula, the most accurate standard for perceptual color
 * difference. Allows optional weighting factors for lightness, chroma, and hue.
 *
 * While most accurate, this is also the most computationally intensive
 * of the metrics.
 *
 * @tparam ColorType1, ColorType2 Any color type convertible to colors::Lab.
 *
 * @param K_L Lightness weighting factor (default: 1.0).
 * @param K_C Chroma weighting factor (default: 1.0).
 * @param K_H Hue weighting factor (default: 1.0).
 *
 * @see qualpal::colors::Lab
 */
class CIEDE2000
{
private:
  double K_L;
  double K_C;
  double K_H;

public:
  /**
   * @brief Construct CIEDE2000 metric with optional weighting factors
   * @param K_L Lightness weighting factor (default: 1.0)
   * @param K_C Chroma weighting factor (default: 1.0)
   * @param K_H Hue weighting factor (default: 1.0)
   */
  explicit CIEDE2000(double K_L = 1.0, double K_C = 1.0, double K_H = 1.0)
    : K_L(K_L)
    , K_C(K_C)
    , K_H(K_H)
  {
    assert((K_L > 0 && K_C > 0 && K_H > 0) &&
           "CIEDE2000 weighting factors must be positive");
  }

  /**
   * @brief Calculate CIEDE2000 color difference
   * @tparam ColorType1 Any color class that can be converted to colors::Lab
   * @tparam ColorType2 Any color class that can be converted to colors::Lab
   * @param c1 First color
   * @param c2 Second color
   * @return CIEDE2000 Delta E value
   */
  template<typename ColorType1, typename ColorType2>
  double operator()(const ColorType1& c1, const ColorType2& c2) const
  {
    using namespace detail;

    colors::Lab x(c1), y(c2);

    double L_hat_prime = (x.l() + y.l()) / 2.0;
    double C1 = std::hypot(x.a(), x.b());
    double C2 = std::hypot(y.a(), y.b());
    double C_hat = (C1 + C2) / 2.0;
    double G = 0.5 * (1 - std::sqrt(std::pow(C_hat, 7) /
                                    (std::pow(C_hat, 7) + std::pow(25.0, 7))));
    double a1_prime = x.a() * (1.0 + G);
    double a2_prime = y.a() * (1.0 + G);
    double C1_prime = std::hypot(a1_prime, x.b());
    double C2_prime = std::hypot(a2_prime, y.b());
    double C_hat_prime = (C1_prime + C2_prime) / 2.0;

    double h1_prime = atan2d(x.b(), a1_prime);

    if (h1_prime < 0) {
      h1_prime += 360;
    }

    double h2_prime = atan2d(y.b(), a2_prime);

    if (h2_prime < 0) {
      h2_prime += 360;
    }

    double H_hat_prime = std::abs(h1_prime - h2_prime) > 180
                           ? (h1_prime + h2_prime + 360) / 2.0
                           : (h1_prime + h2_prime) / 2.0;

    double T = 1.0 - 0.17 * cosd(H_hat_prime - 30) +
               0.24 * cosd(2 * H_hat_prime) + 0.32 * cosd(3 * H_hat_prime + 6) -
               0.20 * cosd(4 * H_hat_prime - 63);

    double delta_h_prime = h2_prime - h1_prime;
    if (std::abs(delta_h_prime) > 180) {
      if (h2_prime <= h1_prime)
        delta_h_prime += 360;
      else
        delta_h_prime -= 360;
    }

    double delta_L_prime = y.l() - x.l();
    double delta_C_prime = C2_prime - C1_prime;

    double delta_H_prime =
      2 * std::sqrt(C1_prime * C2_prime) * sind(delta_h_prime / 2.0);
    double S_L = 1 + (0.015 * std::pow(L_hat_prime - 50, 2)) /
                       std::sqrt(20 + std::pow(L_hat_prime - 50, 2));

    double S_C = 1 + 0.045 * C_hat_prime;
    double S_H = 1 + 0.015 * C_hat_prime * T;

    double delta_theta = 30 * std::exp(-std::pow((H_hat_prime - 275) / 25, 2));

    double R_C = 2 * std::sqrt(std::pow(C_hat_prime, 7) /
                               (std::pow(C_hat_prime, 7) + std::pow(25.0, 7)));

    double R_T = -R_C * sind(2 * delta_theta);
    double out = std::sqrt(square(delta_L_prime / (K_L * S_L)) +
                           square(delta_C_prime / (K_C * S_C)) +
                           square(delta_H_prime / (K_H * S_H)) +
                           R_T * (delta_C_prime / (K_C * S_C)) *
                             (delta_H_prime / (K_H * S_H)));

    assert(out >= 0 && "CIEDE2000 color difference must be non-negative");
    assert(std::isfinite(out) &&
           "CIEDE2000 color difference must not be finite");

    return out;
  }
};
} // namespace metrics
} // namespace qualpal
