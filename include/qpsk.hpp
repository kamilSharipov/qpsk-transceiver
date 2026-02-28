#pragma once

#include "system.hpp"

#include <vector>

namespace qpsk {

constexpr size_t QPSK_STD_SYMBOL_SIZE = 2;

class QPSK {
public:
    std::vector<Complex> modulate(const std::vector<u_int8_t>& bits) const;
    std::vector<double> demodulate(const std::vector<Complex>& symbols) const;
};

} // namespace qpsk
