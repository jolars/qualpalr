#include <Rcpp.h>
#include <qualpal/colors.h>

template<typename T>
std::array<double, 3>
convert(const T& color, const std::string& to)
{
  using namespace qualpal::colors;
  if (to == "rgb") {
    RGB rgb(color);
    return { rgb.r(), rgb.g(), rgb.b() };
  } else if (to == "hsl") {
    HSL hsl(color);
    return { hsl.h(), hsl.s(), hsl.l() };
  } else if (to == "din99d") {
    DIN99d din99d(color);
    return { din99d.l(), din99d.a(), din99d.b() };
  } else if (to == "lab") {
    Lab lab(color);
    return { lab.l(), lab.a(), lab.b() };
  } else if (to == "xyz") {
    XYZ xyz(color);
    return { xyz.x(), xyz.y(), xyz.z() };
  } else {
    Rcpp::stop("Unknown colorspace");
  }
}
