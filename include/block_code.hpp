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
class BlockCode {
static_assert(N == 2 || N == 4 || N == 6 || N == 8 || N == 11,
              "N must be in {2,4,6,8,11}");

public:
    std::bitset<CODEWORD_SIZE> encode(const std::bitset<N>& info_bits) const {
        std::bitset<CODEWORD_SIZE> codeword;

        for (size_t i = 0; i < CODEWORD_SIZE; ++i) {
            bool bit = false;
            auto row = BASE_MATRIX[i];

            for (int j = 0; j < N; ++j) {
                if (info_bits[j] && row[j]) {
                    bit = !bit;
                }
            }

            codeword[i] = bit;
        }

        return codeword;
    }

    std::bitset<N> decode(const std::vector<double>& llrs) const {
        if (llrs.size() != CODEWORD_SIZE) {
            throw std::invalid_argument("lib/block_code.cpp: LLR vector must have 20 elements");
        }

        size_t total = 1ULL << N;
        double best_metric = -1e300;
        std::bitset<N> best_word;

        for (size_t i = 0; i < total; ++i) {
            std::bitset<N> candidate(i);
            std::bitset<CODEWORD_SIZE> codeword = encode(candidate);

            double metric = 0.0;
            for (size_t j = 0; j < CODEWORD_SIZE; ++j) {
                if (codeword[j]) {
                    metric += llrs[j];
                }
            }

            if (metric > best_metric) {
                best_metric = metric;
                best_word = candidate;
            }
        }

        return best_word;
    }
};

} // namespace qpsk
