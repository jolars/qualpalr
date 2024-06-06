#include "colors.h"
#include "distance_matrix.h"
#include "matrix.h"
#include <map>
#include <vector>

namespace qualpal {

std::vector<RGB>
qualpal(const int n,
        std::vector<RGB> rgb_colors,
        const std::map<std::string, double>& cvd = {});

std::vector<RGB>
qualpal(const int n,
        const std::vector<std::string>& hex_colors,
        const std::map<std::string, double>& cvd = {});

std::vector<RGB>
qualpal(const int n,
        const std::array<double, 2>& h_lim = { 0, 360 },
        const std::array<double, 2>& s_lim = { 0, 1 },
        const std::array<double, 2>& l_lim = { 0, 1 },
        const int n_points = 1000,
        const std::map<std::string, double>& cvd = {});

} // namespace qualpal
