#pragma once

#include "system.hpp"
#include <string>

namespace qpsk {

Complex parse_complex(const std::string& s);
std::string format_complex(const Complex& c);

} // namespace qpsk
