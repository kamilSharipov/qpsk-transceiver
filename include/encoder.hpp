#pragma once

#include <vector>
#include <cstdint>
#include <bitset>
#include <array>
#include <stdexcept>

namespace qpsk {

constexpr size_t CODEWORD_SIZE = 20;
constexpr std::array<int, 5> VALID_N_BITS = {2, 4, 6, 8, 11};

constexpr std::array<std::bitset<13>, CODEWORD_SIZE> BASE_MATRIX = {{
    0b1100000000110,
    0b1110000001110,
    0b1001001011111,
    0b1011000010111,
    0b1111000100111,
    0b1100101110111,
    0b1010101011111,
    0b1001100110111,
    0b1101100101111,
    0b1011101001111,
    0b1010011101111,
    0b1110011010111,
    0b1001010111111,
    0b1101010101111,
    0b1000110100101,
    0b1100111101101,
    0b1110111001011,
    0b1001110010011,
    0b1101111100000,
    0b1000011000000
}};

template <int N>
class BlockEncoder {
static_assert(N == 2 || N == 4 || N == 6 || N == 8 || N == 11,
              "N must be in {2,4,6,8,11}");

public:
    std::bitset<CODEWORD_SIZE> encode(const std::bitset<N>& info_bits) const;
};

} // namespace qpsk
