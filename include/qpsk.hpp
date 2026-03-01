#pragma once

#include "system.hpp"
#include "block_code.hpp"

#include <vector>
#include <cstdint>

namespace qpsk {

constexpr size_t QPSK_STD_SYMBOL_SIZE = 2;
constexpr size_t QPSK_SYMBOLS_COUNT   = CODEWORD_SIZE / QPSK_STD_SYMBOL_SIZE;

class QPSK {
public:
    std::vector<Complex> modulate(const std::bitset<CODEWORD_SIZE>& bits) const;
    std::vector<double> demodulate(const std::vector<Complex>& symbols) const;
};

} // namespace qpsk
