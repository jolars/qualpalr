#include "math.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <qualpal/colors.h>
#include <qualpal/matrix.h>
#include <sstream>

namespace qualpal {
namespace colors {

inline double
inverseCompanding(const double v)
{
  return v <= 0.04045 ? v / 12.92 : std::pow((v + 0.055) / 1.055, 2.4);
}

RGB::RGB()
  : r_value(0.0)
  , g_value(0.0)
  , b_value(0.0)
{
}

RGB::RGB(const double r, const double g, const double b)
  : r_value(r)
  , g_value(g)
  , b_value(b)
{
  assert(r >= 0 && r <= 1 && "Red component must be in [0, 1]");
  assert(g >= 0 && g <= 1 && "Green component must be in [0, 1]");
  assert(b >= 0 && b <= 1 && "Blue component must be in [0, 1]");
}

RGB::RGB(const std::string& hex)
{
  std::stringstream ss;

  int red = 0;
  int green = 0;
  int blue = 0;

  // Assert that the string starts with '#'
  assert(hex[0] == '#' && "Hex string must start with '#'");

  // Assert that the string length is either 4 or 7
  assert((hex.length() == 4 || hex.length() == 7) &&
         "Hex string length be 4 or 7");

  if (hex.length() == 4) {
    ss << std::hex << hex.substr(1, 1) << hex.substr(1, 1);
    ss >> red;

    ss.clear();

    ss << std::hex << hex.substr(2, 1) << hex.substr(2, 1);
    ss >> green;

    ss.clear();

    ss << std::hex << hex.substr(3, 1) << hex.substr(3, 1);
    ss >> blue;
  } else if (hex.length() == 7) {
    ss << std::hex << hex.substr(1, 2);
    ss >> red;

    ss.clear();

    ss << std::hex << hex.substr(3, 2);
    ss >> green;

    ss.clear();

    ss << std::hex << hex.substr(5, 2);
    ss >> blue;
  }

  assert(red >= 0 && red <= 255 && "The red part of the hex string is valid");
  assert(green >= 0 && green <= 255 &&
         "The green part of the hex string is valid");
  assert(blue >= 0 && blue <= 255 &&
         "The blue part of the hex string is valid");

  r_value = red / 255.0;
  g_value = green / 255.0;
  b_value = blue / 255.0;
}

RGB::RGB(const HSL& hsl)
{
  double c = (1 - std::abs(2 * hsl.l() - 1)) * hsl.s();
  double h_prime = mod(hsl.h(), 360.0) / 60.0;
  double x = c * (1 - std::abs(mod(h_prime, 2) - 1));

  std::array<double, 3> rgb_prime = { 0, 0, 0 };

  if (h_prime >= 0 && h_prime < 1) {
    rgb_prime = { c, x, 0 };
  } else if (h_prime >= 1 && h_prime < 2) {
    rgb_prime = { x, c, 0 };
  } else if (h_prime >= 2 && h_prime < 3) {
    rgb_prime = { 0, c, x };
  } else if (h_prime >= 3 && h_prime < 4) {
    rgb_prime = { 0, x, c };
  } else if (h_prime >= 4 && h_prime < 5) {
    rgb_prime = { x, 0, c };
  } else if (h_prime >= 5 && h_prime < 6) {
    rgb_prime = { c, 0, x };
  }

  double m = hsl.l() - c / 2.0;

  r_value = std::clamp(rgb_prime[0] + m, 0.0, 1.0);
  g_value = std::clamp(rgb_prime[1] + m, 0.0, 1.0);
  b_value = std::clamp(rgb_prime[2] + m, 0.0, 1.0);
}

RGB::RGB(const Lab& lab)
  : RGB(XYZ(lab))
{
}

RGB::RGB(const XYZ& xyz)
{
  FixedMatrix<double, 3, 3> m = { { 3.2404542, -1.5371385, -0.4985314 },
                                  { -0.9692660, 1.8760108, 0.0415560 },
                                  { 0.0556434, -0.2040259, 1.0572252 } };
  std::array<double, 3> xyz_vec = { xyz.x(), xyz.y(), xyz.z() };

  auto rgb = m * xyz_vec;

  for (auto& val : rgb) {
    if (val > 0.0031308) {
      val = 1.055 * std::pow(val, 1 / 2.4) - 0.055;
    } else {
      val = 12.92 * val;
    }
  }

  r_value = std::clamp(rgb[0], 0.0, 1.0);
  g_value = std::clamp(rgb[1], 0.0, 1.0);
  b_value = std::clamp(rgb[2], 0.0, 1.0);
}

RGB::RGB(const LCHab& lch)
  : RGB(Lab(lch))
{
}

HSL::HSL()
  : h_value(0.0)
  , s_value(0.0)
  , l_value(0.0)
{
}

HSL::HSL(const XYZ& xyz)
  : HSL(RGB(xyz))
{
}

HSL::HSL(const Lab& lab)
  : HSL(XYZ(lab))
{
}

HSL::HSL(const LCHab& lch)
  : HSL(Lab(lch))
{
}

XYZ::XYZ()
  : x_value(0.0)
  , y_value(0.0)
  , z_value(0.0)
{
}

XYZ::XYZ(const double x, const double y, const double z)
  : x_value(x)
  , y_value(y)
  , z_value(z)
{
  assert(x >= 0 && "X component must be non-negative");
  assert(y >= 0 && "Y component must be non-negative");
  assert(z >= 0 && "Z component must be non-negative");
}

XYZ::XYZ(const RGB& rgb)
{
  FixedMatrix<double, 3, 3> m = { { 0.4124564, 0.3575761, 0.1804375 },
                                  { 0.2126729, 0.7151522, 0.0721750 },
                                  { 0.0193339, 0.1191920, 0.9503041 } };

  double r = inverseCompanding(rgb.r());
  double g = inverseCompanding(rgb.g());
  double b = inverseCompanding(rgb.b());

  std::array<double, 3> rgb_vec = { r, g, b };
  auto xyz = m * rgb_vec;

  x_value = xyz[0];
  y_value = xyz[1];
  z_value = xyz[2];

  assert(x_value >= 0 && "X component must be non-negative");
  assert(y_value >= 0 && "Y component must be non-negative");
  assert(z_value >= 0 && "Z component must be non-negative");
}

XYZ::XYZ(const Lab& lab, const std::array<double, 3>& white_point)
{
  const double l = lab.l();
  const double a = lab.a();
  const double b = lab.b();

  const double epsilon = 216.0 / 24389.0;
  const double kappa = 24389.0 / 27.0;

  double fy = (l + 16.0) / 116.0;
  double fx = a / 500.0 + fy;
  double fz = fy - b / 200.0;

  double xr =
    std::pow(fx, 3) > epsilon ? std::pow(fx, 3) : (116.0 * fx - 16.0) / kappa;
  double yr = l > kappa * epsilon ? std::pow((l + 16.0) / 116.0, 3) : l / kappa;
  double zr =
    std::pow(fz, 3) > epsilon ? std::pow(fz, 3) : (116.0 * fz - 16.0) / kappa;

  x_value = std::max(xr * white_point[0], 0.0);
  y_value = std::max(yr * white_point[1], 0.0);
  z_value = std::max(zr * white_point[2], 0.0);
}

XYZ::XYZ(const HSL& hsl)
  : XYZ(RGB(hsl))
{
}

XYZ::XYZ(const LCHab& lch, const std::array<double, 3>& white_point)
  : XYZ(Lab(lch), white_point)
{
}

std::string
RGB::hex() const
{
  std::stringstream ss;

  ss << "#" << std::hex << std::setfill('0');
  ss << std::setw(2) << static_cast<int>(std::round(this->r() * 255));
  ss << std::setw(2) << static_cast<int>(std::round(this->g() * 255));
  ss << std::setw(2) << static_cast<int>(std::round(this->b() * 255));

  return ss.str();
}

DIN99d::DIN99d()
  : l_value(0.0)
  , a_value(0.0)
  , b_value(0.0)
{
}

DIN99d::DIN99d(const double l, const double a, const double b)
  : l_value(l)
  , a_value(a)
  , b_value(b)
{
  assert(l >= 0 && l <= 100 && "Lightness must be in [0, 100]");
  assert(a >= -128 && a <= 127 && "Green-red component must be in [-128, 127]");
  assert(b >= -128 && b <= 127 &&
         "Blue-yellow component must be in [-128, 127]");
}

DIN99d::DIN99d(const XYZ& xyz, const std::array<double, 3>& white_point)
{
  double x_prime = 1.12 * xyz.x() - 0.12 * xyz.z();
  double xw_prime = 1.12 * white_point[0] - 0.12 * white_point[2];

  XYZ xyz_prime(x_prime, xyz.y(), xyz.z());
  std::array<double, 3> white_prime = { xw_prime,
                                        white_point[1],
                                        white_point[2] };

  Lab lab(xyz_prime, white_prime);

  double l = lab.l();
  double a = lab.a();
  double b = lab.b();

  double u = 50 * M_PI / 180.0;
  double e = a * std::cos(u) + b * std::sin(u);
  double f = 1.14 * (b * std::cos(u) - a * std::sin(u));
  double g = std::hypot(e, f);

  double c99d = 22.5 * std::log1p(0.06 * g);
  double h99d = std::atan2(f, e) + 50 * M_PI / 180.0;

  l_value = 325.22 * std::log1p(0.0036 * l);
  a_value = c99d * std::cos(h99d);
  b_value = c99d * std::sin(h99d);

  l_value = std::clamp(l_value, 0.0, 100.0);
  a_value = std::clamp(a_value, -128.0, 127.0);
  b_value = std::clamp(b_value, -128.0, 127.0);
}

DIN99d::DIN99d(const RGB& rgb)
  : DIN99d(XYZ(rgb))
{
}

DIN99d::DIN99d(const HSL& hsl)
  : DIN99d(XYZ(hsl))
{
}

DIN99d::DIN99d(const Lab& lab)
  : DIN99d(XYZ(lab))
{
}

HSL::HSL(const double h, const double s, const double l)
  : h_value(h)
  , s_value(s)
  , l_value(l)
{
  assert(h >= 0 && h < 360 && "Hue must be in [0, 360)");
  assert(s >= 0 && s <= 1 && "Saturation must be in [0, 1]");
  assert(l >= 0 && l <= 1 && "Lightness must be in [0, 1]");
}

HSL::HSL(const RGB& rgb)
{
  const double r = rgb.r();
  const double g = rgb.g();
  const double b = rgb.b();

  const double x_max = std::max({ r, g, b });
  const double x_min = std::min({ r, g, b });
  const double v = x_max;
  const double c = x_max - x_min;

  this->l_value = 0.5 * (x_max + x_min);

  double h_prime = 0;

  if (c == 0) {
    h_prime = 0.0;
  } else if (v == r) {
    h_prime = mod((g - b) / c, 6.0);
  } else if (v == g) {
    h_prime = (b - r) / c + 2;
  } else if (v == b) {
    h_prime = (r - g) / c + 4;
  }

  this->h_value = h_prime * 60;
  this->s_value =
    (l_value == 1 || l_value == 0) ? 0 : c / (1.0 - std::abs(2 * v - c - 1));

  h_value = std::clamp(h_value, 0.0, 360.0);
  s_value = std::clamp(s_value, 0.0, 1.0);
  l_value = std::clamp(l_value, 0.0, 1.0);
}

Lab::Lab()
  : l_value(0.0)
  , a_value(0.0)
  , b_value(0.0)
{
}

Lab::Lab(const double l, const double a, const double b)
  : l_value(l)
  , a_value(a)
  , b_value(b)
{
  assert(l >= 0 && l <= 100 && "Lightness must be in [0, 100]");
  assert(a >= -128 && a <= 127 && "Green-red component must be in [-128, 127]");
  assert(b >= -128 && b <= 127 &&
         "Blue-yellow component must be in [-128, 127]");
}

Lab::Lab(const XYZ& xyz, const std::array<double, 3>& white_point)
{
  double x = xyz.x();
  double y = xyz.y();
  double z = xyz.z();

  // Actual CIE standard values
  double epsilon = 0.008856;
  double kappa = 903.3;

  double xr = x / white_point[0];
  double yr = y / white_point[1];
  double zr = z / white_point[2];

  double fx = xr > epsilon ? std::cbrt(xr) : (kappa * xr + 16.0) / 116.0;
  double fy = yr > epsilon ? std::cbrt(yr) : (kappa * yr + 16.0) / 116.0;
  double fz = zr > epsilon ? std::cbrt(zr) : (kappa * zr + 16.0) / 116.0;

  l_value = std::clamp(116.0 * fy - 16.0, 0.0, 100.0);
  a_value = std::clamp(500.0 * (fx - fy), -128.0, 127.0);
  b_value = std::clamp(200.0 * (fy - fz), -128.0, 127.0);
};

Lab::Lab(const RGB& rgb)
  : Lab(XYZ(rgb))
{
}

Lab::Lab(const HSL& hsl)
  : Lab(XYZ(hsl))
{
}

Lab::Lab(const LCHab& lch)
  : l_value(lch.l())
  , a_value(lch.c() * std::cos(lch.h() * M_PI / 180.0))
  , b_value(lch.c() * std::sin(lch.h() * M_PI / 180.0))
{
}

LCHab::LCHab()
  : l_value(0.0)
  , c_value(0.0)
  , h_value(0.0)
{
}

LCHab::LCHab(const double l, const double c, const double h)
  : l_value(l)
  , c_value(c)
  , h_value(h)
{
  assert(l >= 0 && l <= 100 && "Lightness must be in [0, 100]");
  assert(c >= 0 && "Chroma must be non-negative");
  assert(h >= 0 && h < 360 && "Hue must be in [0, 360)");
}

LCHab::LCHab(const Lab& lab)
{
  l_value = lab.l();
  c_value = std::hypot(lab.a(), lab.b());

  double angle = std::atan2(lab.b(), lab.a()) * 180.0 / M_PI;

  h_value = angle >= 0 ? angle : angle + 360.0;
}

LCHab::LCHab(const RGB& rgb)
  : LCHab(Lab(rgb))
{
}

LCHab::LCHab(const HSL& hsl)
  : LCHab(Lab(hsl))
{
}

LCHab::LCHab(const XYZ& xyz, const std::array<double, 3>& white_point)
  : LCHab(Lab(xyz, white_point))
{
}

} // namespace colors
} // namespace qualpal
