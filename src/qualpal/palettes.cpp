#include "palettes.h"
#include "palettes_data.h"
#include <map>
#include <stdexcept>

namespace qualpal {

void
validatePalette(const std::string& palette)
{
  std::string delimiter = ":";
  size_t pos = palette.find(delimiter);

  if (pos == std::string::npos) {
    throw std::invalid_argument(
      "Invalid palette format. Expected format: 'package:palette'");
  }

  std::string pkg = palette.substr(0, pos);
  std::string pal = palette.substr(pos + delimiter.length());

  if (color_palettes.find(pkg) == color_palettes.end()) {
    throw std::invalid_argument("Package '" + pkg + "' not found");
  }

  if (color_palettes.at(pkg).find(pal) == color_palettes[pkg].end()) {
    throw std::invalid_argument("Palette '" + pal + "' not found in package '" +
                                pkg + "'");
  }
}

std::vector<std::string>
getPalette(const std::string& palette)
{
  validatePalette(palette);

  std::string delimiter = ":";
  size_t pos = palette.find(delimiter);
  std::string pkg = palette.substr(0, pos);
  std::string pal = palette.substr(pos + delimiter.length());

  return color_palettes[pkg][pal];
}

} // namespace qualpal
