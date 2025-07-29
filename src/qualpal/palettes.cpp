#include "palettes.h"
#include "palettes_data.h"
#include <map>
#include <stdexcept>

namespace qualpal {

void
validatePalette(const std::string& palette)
{
  std::string delimiter = ":";
  std::size_t pos = palette.find(delimiter);

  if (pos == std::string::npos) {
    throw std::invalid_argument(
      "Invalid palette format. Expected format: 'package:palette'");
  }

  std::string pkg = palette.substr(0, pos);
  std::string pal = palette.substr(pos + delimiter.length());

  if (COLOR_PALETTES.find(pkg) == COLOR_PALETTES.end()) {
    throw std::invalid_argument("Package '" + pkg + "' not found");
  }

  if (COLOR_PALETTES.at(pkg).find(pal) == COLOR_PALETTES[pkg].end()) {
    throw std::invalid_argument("Palette '" + pal + "' not found in package '" +
                                pkg + "'");
  }
}

std::vector<std::string>
getPalette(const std::string& palette)
{
  validatePalette(palette);

  std::string delimiter = ":";
  std::size_t pos = palette.find(delimiter);
  std::string pkg = palette.substr(0, pos);
  std::string pal = palette.substr(pos + delimiter.length());

  return COLOR_PALETTES[pkg][pal];
}

std::map<std::string, std::vector<std::string>>
listAvailablePalettes()
{
  std::map<std::string, std::vector<std::string>> result;
  for (const auto& pkg_pair : COLOR_PALETTES) {
    const auto& pkg = pkg_pair.first;
    const auto& palettes = pkg_pair.second;
    std::vector<std::string> pal_names;
    for (const auto& pal_pair : palettes) {
      pal_names.push_back(pal_pair.first);
    }
    result[pkg] = pal_names;
  }
  return result;
}

} // namespace qualpal
