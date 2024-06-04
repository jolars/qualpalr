#pragma once

#include <array>
#include <string>

namespace qualpal {

// forward declarations
class HSL;
class XYZ;
class Lab;

class RGB
{
private:
  double r_value;
  double g_value;
  double b_value;

public:
  RGB(const double r, const double g, const double b);
  RGB(const std::array<double, 3> rgb);
  RGB(const std::string& hex);
  RGB(const HSL& hsl);
  RGB(const XYZ& xyz);
  std::string hex() const;
  double r() const { return r_value; }
  double g() const { return g_value; }
  double b() const { return b_value; }
};

class HSL
{
private:
  double h_value;
  double s_value;
  double l_value;

public:
  HSL(const double h, const double s, const double l);
  HSL(const RGB& rgb);
  double h() const { return h_value; }
  double s() const { return s_value; }
  double l() const { return l_value; }
};

class XYZ
{
private:
  double x_value;
  double y_value;
  double z_value;

public:
  XYZ(const double x, const double y, const double z);
  XYZ(const RGB& rgb);
  XYZ(const Lab& lab,
      const std::array<double, 3>& white_point = { 0.95047, 1, 1.08883 });
  double x() const { return x_value; }
  double y() const { return y_value; }
  double z() const { return z_value; }
};

class DIN99d
{
private:
  double l_value;
  double a_value;
  double b_value;

public:
  DIN99d(const double l, const double a, const double b);
  DIN99d(const XYZ& xyz);
  DIN99d(const RGB& rgb);
  double l() const { return l_value; }
  double a() const { return a_value; }
  double b() const { return b_value; }
};

class Lab
{
private:
  double l_value;
  double a_value;
  double b_value;

public:
  Lab(const double l, const double a, const double b);
  Lab(const XYZ& xyz,
      const std::array<double, 3>& white_point = { 0.95047, 1, 1.08883 });
  double l() const { return l_value; }
  double a() const { return a_value; }
  double b() const { return b_value; }
};

} // namespace qualpal
