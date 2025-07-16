#pragma once

#include <array>
#include <string>

namespace qualpal {

bool
isValidHexColor(const std::string& color);

void
validateHslRanges(const std::array<double, 2>& h_lim,
                  const std::array<double, 2>& s_lim,
                  const std::array<double, 2>& l_lim);

}
