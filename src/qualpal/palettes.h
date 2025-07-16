#include <map>
#include <string>
#include <vector>

namespace qualpal {

void
validatePalette(const std::string& palette);

std::vector<std::string>
getPalette(const std::string& palette);

std::map<std::string, std::vector<std::string>>
listAvailablePalettes();

} // namespace qualpal
