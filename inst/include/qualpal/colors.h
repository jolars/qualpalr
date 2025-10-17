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
class LCHab;

/**
 * @brief RGB color representation with values in [0,1] range
 *
 * Represents a color in the standard Red-Green-Blue (RGB) color space. All
 * channel values are normalized to the range [0, 1].
 *
 * Provides conversion constructors for HSL, XYZ, Lab, and LCHab color spaces.
 * Also supports construction from hexadecimal color strings in "#RRGGBB" or
 * "#RGB" format.
 *
 * @code{.cpp}
 * // Create RGB from normalized values
 * RGB red(1.0, 0.0, 0.0);
 *
 * // Create from hex string (6-digit and 3-digit)
 * RGB blue("#0000ff");
 * RGB magenta("#f0f");
 *
 * // Convert to hex
 * std::string hex = red.hex(); // "#ff0000"
 * @endcode
 *
 * @throws std::invalid_argument if hex string format is invalid.
 * @see HSL, XYZ, Lab, LCHab for conversions.
 */
class RGB
{
private:
  double r_value;
  double g_value;
  double b_value;

public:
  /**
   * @brief Default constructor initializes RGB to black (0,0,0)
   */
  RGB();

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
   * @brief Construct RGB from hexadecimal color string literal
   * @param hex Hex color string in format "#RRGGBB" or "#RGB"
   * @throws std::invalid_argument if hex format is invalid
   */
  RGB(const char* hex)
    : RGB(std::string(hex)) {};

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
   * @brief Construct RGB from LCHab color
   * @param lch LCHab color to convert from
   */
  RGB(const LCHab& lch);

  /**
   * @brief Equality operator for RGB colors
   * @param other Other RGB color to compare with
   * @return true if both RGB colors are equal, false otherwise
   */
  bool operator==(const RGB& other) const
  {
    return (r_value == other.r_value) && (g_value == other.g_value) &&
           (b_value == other.b_value);
  }

  /**
   * @brief Inequality operator for RGB colors
   * @param other Other RGB color to compare with
   * @return true if RGB colors are not equal, false otherwise
   */
  bool operator!=(const RGB& other) const { return !(*this == other); }

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
 * Represents a color in the HSL color space:
 * - Hue: [0, 360) degrees (0 = red, 120 = green, 240 = blue)
 * - Saturation: [0, 1] (0 = gray, 1 = fully saturated)
 * - Lightness: [0, 1] (0 = black, 1 = white)
 *
 * Provides conversion constructors for RGB, XYZ, Lab, and LCHab color spaces.
 *
 * @code{.cpp}
 * HSL orange(30, 1.0, 0.5);    // Pure orange
 * HSL gray(0, 0.0, 0.5);       // Medium gray
 * HSL from_rgb(RGB(0.5, 0.2, 0.7));
 * @endcode
 *
 * @see RGB, XYZ, Lab, LCHab for conversions.
 */
class HSL
{
private:
  double h_value;
  double s_value;
  double l_value;

public:
  /**
   * @brief Default constructor initializes HSL to black (0,0,0)
   */
  HSL();

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

  /**
   * @brief Construct HSL from LCHab color
   * @param lch LCHab color to convert from
   */
  HSL(const LCHab& lch);

  /**
   * @brief Equality operator for HSL colors
   * @param other Other HSL color to compare with
   * @return true if both HSL colors are equal, false otherwise
   */
  bool operator==(const HSL& other) const
  {
    return (h_value == other.h_value) && (s_value == other.s_value) &&
           (l_value == other.l_value);
  }

  /**
   * @brief Inequality operator for HSL colors
   * @param other Other HSL color to compare with
   * @return true if HSL colors are not equal, false otherwise
   */
  bool operator!=(const HSL& other) const { return !(*this == other); }

  /** @brief Get hue in degrees [0,360) */
  double h() const { return h_value; }
  /** @brief Get saturation [0,1] */
  double s() const { return s_value; }
  /** @brief Get lightness [0,1] */
  double l() const { return l_value; }
};

/**
 * @brief XYZ color representation (CIE 1931 color space).
 *
 * Represents a color in the device-independent CIE 1931 XYZ color space.
 * Used as an intermediate for conversions between RGB and perceptually uniform
 * spaces (Lab, LCHab, DIN99d).
 *
 * - X: [0, ~0.95] (roughly red-like)
 * - Y: [0, 1] (luminance)
 * - Z: [0, ~1.09] (roughly blue-like)
 *
 * Most conversions use the D65 reference white point by default.
 *
 * @code{.cpp}
 * // Create XYZ from RGB
 * XYZ xyz_from_rgb(RGB(0.5, 0.2, 0.7));
 *
 * // Modify whitepoint
 * XYZ xyz_custom_white(0.4, 0.5, 0.6, { 0.96422, 1.0, 0.82521 }); // D50
 * @endcode
 *
 * @see RGB, Lab, LCHab, DIN99d for conversions.
 */
class XYZ
{
private:
  double x_value;
  double y_value;
  double z_value;

public:
  /**
   * @brief Default constructor initializes XYZ to (0,0,0)
   */
  XYZ();

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

  /**
   * @brief Construct XYZ from LCHab color
   * @param lch LCHab color to convert from
   * @param white_point Reference white point (default: D65)
   */
  XYZ(const LCHab& lch,
      const std::array<double, 3>& white_point = { 0.95047, 1, 1.08883 });

  /**
   * @brief equality operator for XYZ colors
   * @param other Other XYZ color to compare with
   * @return true if both XYZ colors are equal, false otherwise
   */
  bool operator==(const XYZ& other) const
  {
    return (x_value == other.x_value) && (y_value == other.y_value) &&
           (z_value == other.z_value);
  }

  /**
   * @brief inequality operator for XYZ colors
   * @param other Other XYZ color to compare with
   * @return true if XYZ colors are not equal, false otherwise
   */
  bool operator!=(const XYZ& other) const { return !(*this == other); }

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
 * @brief DIN99d color space representation.
 *
 * Perceptually uniform color space optimized for color difference
 * calculations, especially for small differences. Used internally by qualpal
 * for palette generation.
 *
 * - l: Lightness [0, 100]
 * - a: Green-red axis [-128, 127]
 * - b: Blue-yellow axis [-128, 127]
 *
 * Provides conversion constructors for RGB, HSL, Lab, and XYZ.
 * Most conversions use the D65 reference white point by default.
 *
 * @see Lab, RGB, HSL, XYZ for conversions.
 */
class DIN99d
{
private:
  double l_value;
  double a_value;
  double b_value;

public:
  /**
   * @brief Default constructor initializes DIN99d to black (0,0,0)
   */
  DIN99d();

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

  /**
   * @brief equality operator for DIN99d colors
   * @param other Other DIN99d color to compare with
   * @return true if both DIN99d colors are equal, false otherwise
   */
  bool operator==(const DIN99d& other) const
  {
    return (l_value == other.l_value) && (a_value == other.a_value) &&
           (b_value == other.b_value);
  }

  /**
   * @brief inequality operator for DIN99d colors
   * @param other Other DIN99d color to compare with
   * @return true if DIN99d colors are not equal, false otherwise
   */
  bool operator!=(const DIN99d& other) const { return !(*this == other); }

  /** @brief Get lightness component */
  double l() const { return l_value; }
  /** @brief Get green-red component */
  double a() const { return a_value; }
  /** @brief Get blue-yellow component */
  double b() const { return b_value; }
};

/**
 * @brief Lab color space representation (CIE L*a*b*).
 *
 * Perceptually uniform color space where Euclidean distance
 * approximates perceived color difference.
 *
 * - L: Lightness [0, 100] (0 = black, 100 = white)
 * - a: Green-red axis [-128, 127] (negative = green, positive = red)
 * - b: Blue-yellow axis [-128, 127] (negative = blue, positive = yellow)
 *
 * Provides conversion constructors for RGB, HSL, XYZ, and LCHab.
 * Most conversions use the D65 reference white point by default.
 *
 * @see RGB, HSL, XYZ, LCHab for conversions.
 */
class Lab
{
private:
  double l_value;
  double a_value;
  double b_value;

public:
  /**
   * @brief Default constructor initializes Lab to black (0,0,0)
   */
  Lab();

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

  /**
   * @brief Construct Lab from LCHab color
   * @param lch LCHab color to convert from
   */
  Lab(const LCHab& lch);

  /**
   * @brief Equality operator for Lab colors
   * @param other Other Lab color to compare with
   * @return true if both Lab colors are equal, false otherwise
   */
  bool operator==(const Lab& other) const
  {
    return (l_value == other.l_value) && (a_value == other.a_value) &&
           (b_value == other.b_value);
  }

  /** @brief Get lightness [0,100] */
  double l() const { return l_value; }
  /** @brief Get green-red component */
  double a() const { return a_value; }
  /** @brief Get blue-yellow component */
  double b() const { return b_value; }
};

/**
 * @brief LCHab color space representation (CIE L*C*h)
 *
 * @brief LCHab color space representation (CIE L*C*h).
 *
 * Cylindrical representation of Lab color space:
 * - L: Lightness [0, 100]
 * - C: Chroma [0, ∞)
 * - H: Hue in degrees [0, 360)
 *
 * Useful for intuitive manipulation of color lightness, chroma, and hue.
 * Provides conversion constructors for Lab, RGB, HSL, and XYZ.
 * Most conversions use the D65 reference white point by default.
 *
 * @see Lab, RGB, HSL, XYZ for conversions.
 */
class LCHab
{
private:
  double l_value;
  double c_value;
  double h_value;

public:
  /**
   * @brief Default constructor initializes LCHab to black (0,0,0)
   */
  LCHab();

  /**
   * @brief Construct LCHab from component values
   * @param l Lightness [0,100]
   * @param c Chroma [0,∞)
   * @param h Hue in degrees [0,360)
   */
  LCHab(const double l, const double c, const double h);

  /**
   * @brief Construct LCHab from Lab color
   * @param lab Lab color to convert from
   */
  LCHab(const Lab& lab);

  /** @brief Construct LCHab from RGB color
   * @param rgb RGB color to convert from
   */
  LCHab(const RGB& rgb);

  /**
   * @brief Construct LCHab from HSL color
   * @param hsl HSL color to convert from
   */
  LCHab(const HSL& hsl);

  /**
   * @brief Construct LCHab from XYZ color
   * @param xyz XYZ color to convert from
   * @param white_point Reference white point (default: D65)
   */
  LCHab(const XYZ& xyz,
        const std::array<double, 3>& white_point = { 0.95047, 1, 1.08883 });

  /**
   * @brief equality operator for LCHab colors
   * @param other Other LCHab color to compare with
   * @return true if both LCHab colors are equal, false otherwise
   */
  bool operator==(const LCHab& other) const
  {
    return (l_value == other.l_value) && (c_value == other.c_value) &&
           (h_value == other.h_value);
  }

  /**
   * @brief inequality operator for LCHab colors
   * @param other Other LCHab color to compare with
   * @return true if LCHab colors are not equal, false otherwise
   */
  bool operator!=(const LCHab& other) const { return !(*this == other); }

  /** @brief Get lightness [0,100] */
  double l() const { return l_value; }
  /** @brief Get chroma [0,∞) */
  double c() const { return c_value; }
  /** @brief Get hue in degrees [0,360) */
  double h() const { return h_value; }
};

} // namespace colors
} // namespace qualpal
