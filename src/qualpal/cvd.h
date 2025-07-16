#include <cmath>
#include <qualpal/colors.h>
#include <string_view>

namespace qualpal {

colors::RGB
simulateCvd(const colors::RGB& rgb,
            const std::string_view cvd_type,
            double cvd_severity);

}
