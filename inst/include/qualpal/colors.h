/**
 * @file
 * @brief Color representation classes
 *
 * This file defines classes for representing colors in various color spaces:
 * RGB, HSL, XYZ, Lab, and DIN99d. Each class provides constructors for
 * converting between color spaces and methods for accessing color values.
 * The RGB class also includes methods for creating colors from hex strings
 * and converting RGB values to hex format.
 */

#pragma once

#include <array>
#include <string>

namespace qualpal {

/**
 * @brief Color representation classes for various color spaces
 *
 * This namespace provides classes for representing colors in different color
 * spaces commonly used in color science and palette generation. All classes
 * support seamless conversion between color spaces through constructor
 * overloads.
 *
 * **Supported Color Spaces:**
 * - **RGB**: Standard red-green-blue with values normalized to [0,1]
 * - **HSL**: Hue-saturation-lightness for intuitive color manipulation
 * - **XYZ**: CIE 1931 device-independent intermediate color space
 * - **Lab**: CIE L*a*b* perceptually uniform color space
 * - **DIN99d**: Optimized perceptually uniform space for color differences
 *
 * **Usage Pattern:**
 * @code{.cpp}
 * // Create colors in any space
 * RGB red(1.0, 0.0, 0.0);
 * HSL orange(30, 1.0, 0.5);
 *
 * // Convert between spaces automatically
 * RGB red_from_hsl(orange);  // Constructor conversion
 * Lab red_lab(red);          // Chain conversions
 *
 * // Use with palette algorithms
 * std::vector<DIN99d> palette_colors;
 * auto indices = farthestPoints(5, palette_colors);
 * @endcode
 *
 * All color classes provide:
 * - Conversion constructors from other color spaces
 * - Accessor methods for individual components
 * - Consistent naming (r(), g(), b() for RGB; h(), s(), l() for HSL, etc.)
 *
 * @see metrics namespace for color difference calculations
 * @see farthestPoints() for palette selection algorithms
 */
namespace colors {

// forward declarations
class HSL;
class XYZ;
class Lab;
class DIN99d;

/**
 * @brief RGB color representation with values in [0,1] range
 *
 * The RGB class provides conversion between RGB and other color spaces
 * including HSL, XYZ, and Lab. All RGB values are normalized to [0,1].
 *
 * @code{.cpp}
 * // Create RGB from normalized values
 * RGB red(1.0, 0.0, 0.0);
 *
 * // Create from hex string
 * RGB blue("#0000ff");
 *
 * // Convert to hex
 * std::string hex = red.hex(); // "#ff0000"
 * @endcode
 */
class RGB
{
private:
  double r_value;
  double g_value;
  double b_value;

public:
  /**
   * @brief Construct RGB from normalized values
   * @param r Red component [0,1]
   * @param g Green component [0,1]
   * @param b Blue component [0,1]
   */
  RGB(const double r, const double g, const double b);

  /**
   * @brief Construct RGB from array of normalized values
   * @param rgb Array containing [r, g, b] values in [0,1]
   */
  RGB(const std::array<double, 3> rgb);

  /**
   * @brief Construct RGB from hexadecimal color string
   * @param hex Hex color string in format "#RRGGBB" or "#RGB"
   * @throws std::invalid_argument if hex format is invalid
   *
   * @code{.cpp}
   * RGB red("#ff0000");     // 6-digit format
   * RGB blue("#00f");       // 3-digit format
   * @endcode
   */
  RGB(const std::string& hex);

  /**
   * @brief Construct RGB from HSL color
   * @param hsl HSL color to convert from
   */
  RGB(const HSL& hsl);

  /**
   * @brief Construct RGB from XYZ color
   * @param xyz XYZ color to convert from
   */
  RGB(const XYZ& xyz);

  /**
   * @brief Construct RGB from Lab color
   * @param lab Lab color to convert from
   */
  RGB(const Lab& lab);

  /**
   * @brief Convert RGB to hexadecimal string
   * @return Hex color string in format "#rrggbb"
   *
   * @code{.cpp}
   * RGB red(1.0, 0.0, 0.0);
   * std::string hex = red.hex(); // "#ff0000"
   * @endcode
   */
  std::string hex() const;

  /** @brief Get red component [0,1] */
  double r() const { return r_value; }
  /** @brief Get green component [0,1] */
  double g() const { return g_value; }
  /** @brief Get blue component [0,1] */
  double b() const { return b_value; }
};

/**
 * @brief HSL (Hue, Saturation, Lightness) color representation
 *
 * HSL color space with hue in degrees [0,360), saturation and lightness in
 * [0,1]. Provides intuitive color manipulation and conversion to/from RGB.
 *
 * @code{.cpp}
 * HSL orange(30, 1.0, 0.5);    // Pure orange
 * HSL gray(0, 0.0, 0.5);       // Medium gray
 * @endcode
 */
class HSL
{
private:
  double h_value;
  double s_value;
  double l_value;

public:
  /**
   * @brief Construct HSL from component values
   * @param h Hue in degrees [0,360)
   * @param s Saturation [0,1]
   * @param l Lightness [0,1]
   */
  HSL(const double h, const double s, const double l);

  /**
   * @brief Construct HSL from RGB color
   * @param rgb RGB color to convert from
   */
  HSL(const RGB& rgb);

  /**
   * @brief Construct HSL from XYZ color
   * @param xyz XYZ color to convert from
   */
  HSL(const XYZ& xyz);

  /**
   * @brief Construct HSL from Lab color
   * @param lab Lab color to convert from
   */
  HSL(const Lab& lab);

  /** @brief Get hue in degrees [0,360) */
  double h() const { return h_value; }
  /** @brief Get saturation [0,1] */
  double s() const { return s_value; }
  /** @brief Get lightness [0,1] */
  double l() const { return l_value; }
};

/**
 * @brief XYZ color representation (CIE 1931 color space)
 *
 * Device-independent color space that serves as an intermediate
 * for conversions between RGB and perceptually uniform spaces.
 */
class XYZ
{
private:
  double x_value;
  double y_value;
  double z_value;

public:
  /**
   * @brief Construct XYZ from component values
   * @param x X component (roughly red-like)
   * @param y Y component (luminance)
   * @param z Z component (roughly blue-like)
   */
  XYZ(const double x, const double y, const double z);

  /**
   * @brief Construct XYZ from RGB color
   * @param rgb RGB color to convert from
   */
  XYZ(const RGB& rgb);

  /**
   * @brief Construct XYZ from Lab color
   * @param lab Lab color to convert from
   * @param white_point Reference white point (default: D65)
   */
  XYZ(const Lab& lab,
      const std::array<double, 3>& white_point = { 0.95047, 1, 1.08883 });

  /**
   * @brief Construct XYZ from HSL color
   * @param hsl HSL color to convert from
   */
  XYZ(const HSL& hsl);

  /** @brief Get X component */
  double x() const { return x_value; }
  /** @brief Get Y component (luminance) */
  double y() const { return y_value; }
  /** @brief Get Z component */
  double z() const { return z_value; }
};

/**
 * @brief DIN99d color space representation
 *
 * Perceptually uniform color space optimized for color difference
 * calculations. Used internally by qualpal for palette generation.
 *
 * DIN99d provides better perceptual uniformity than Lab color space,
 * particularly for small color differences.
 */
class DIN99d
{
private:
  double l_value;
  double a_value;
  double b_value;

public:
  /**
   * @brief Construct DIN99d from component values
   * @param l Lightness component
   * @param a Green-red component
   * @param b Blue-yellow component
   */
  DIN99d(const double l, const double a, const double b);

  /**
   * @brief Construct DIN99d from RGB color
   * @param rgb RGB color to convert from
   */
  DIN99d(const RGB& rgb);

  /**
   * @brief Construct DIN99d from HSL color
   * @param hsl HSL color to convert from
   */
  DIN99d(const HSL& hsl);

  /**
   * @brief Construct DIN99d from Lab color
   * @param lab Lab color to convert from
   */
  DIN99d(const Lab& lab);

  /**
   * @brief Construct DIN99d from XYZ color
   * @param xyz XYZ color to convert from
   * @param white_point Reference white point (default: D65)
   */
  DIN99d(const XYZ& xyz,
         const std::array<double, 3>& white_point = { 0.95047, 1, 1.08883 });

  /** @brief Get lightness component */
  double l() const { return l_value; }
  /** @brief Get green-red component */
  double a() const { return a_value; }
  /** @brief Get blue-yellow component */
  double b() const { return b_value; }
};

/**
 * @brief Lab color space representation (CIE L*a*b*)
 *
 * Perceptually uniform color space where Euclidean distance
 * approximates perceived color difference.
 *
 * - L: lightness (0 = black, 100 = white)
 * - a: green-red axis (negative = green, positive = red)
 * - b: blue-yellow axis (negative = blue, positive = yellow)
 */
class Lab
{
private:
  double l_value;
  double a_value;
  double b_value;

public:
  /**
   * @brief Construct Lab from component values
   * @param l Lightness [0,100]
   * @param a Green-red axis [-128,127]
   * @param b Blue-yellow axis [-128,127]
   */
  Lab(const double l, const double a, const double b);

  /**
   * @brief Construct Lab from RGB color
   * @param rgb RGB color to convert from
   */
  Lab(const RGB& rgb);

  /**
   * @brief Construct Lab from HSL color
   * @param hsl HSL color to convert from
   */
  Lab(const HSL& hsl);

  /**
   * @brief Construct Lab from XYZ color
   * @param xyz XYZ color to convert from
   * @param white_point Reference white point (default: D65)
   */
  Lab(const XYZ& xyz,
      const std::array<double, 3>& white_point = { 0.95047, 1, 1.08883 });

  /** @brief Get lightness [0,100] */
  double l() const { return l_value; }
  /** @brief Get green-red component */
  double a() const { return a_value; }
  /** @brief Get blue-yellow component */
  double b() const { return b_value; }
};

} // namespace colors
} // namespace qualpal
