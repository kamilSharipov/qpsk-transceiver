#include "block_code.hpp"
#include "system.hpp"

#include <stdexcept>
#include <vector>
#include <cmath>
#include <iostream>

namespace qpsk {

BlockCode::BlockCode(int n) : n_(n) {
    if (n != 2 && n != 4 && n != 6 && n != 8 && n != 11) {
        throw std::invalid_argument("lib/block_code.cpp: n must be in {2,4,6,8,11}");
    }

    G_.resize(PUCCH_STD_BIT_SIZE, std::vector<uint8_t>(n));

    for (size_t i = 0; i < PUCCH_STD_BIT_SIZE; ++i) {
        for (size_t j = 0; j < n; ++j) {
            G_[i][j] = base_matrix[i][j];
        }
    }
}

std::vector<uint8_t> BlockCode::encode(const std::vector<bool>& info_bits) const {
    if (static_cast<int>(info_bits.size()) != n_) {
        throw std::invalid_argument("lib/block_code.cpp: input size mismatch");
    }

    std::vector<uint8_t> codeword(PUCCH_STD_BIT_SIZE, 0);
    for (size_t i = 0; i < PUCCH_STD_BIT_SIZE; ++i) {
        uint8_t sum = 0;

        for (size_t j = 0; j < n_; ++j) {
            sum ^= (info_bits[j] ? G_[i][j] : 0);
        }

        codeword[i] = sum;
    }

    return codeword;
}

std::vector<bool> BlockCode::decode(const std::vector<double>& llrs) const {
    if (llrs.size() != PUCCH_STD_BIT_SIZE) {
        throw std::invalid_argument("lib/block_code.cpp: LLR vector must have 20 elements");
    }

    size_t total = 1ULL << n_;
    double best_metric = -1e300;
    std::vector<bool> best_word(n_);

    for (size_t i = 0; i < total; ++i) {
        std::vector<bool> candidate(n_);
        for (size_t j = 0; j < n_; ++j) {
            candidate[j] = (i >> j) & 1;
        }

        std::vector<uint8_t> codeword = encode(candidate);

        double metric = 0.0;
        for (size_t j = 0; j < PUCCH_STD_BIT_SIZE; ++j) {
            metric += static_cast<double>(codeword[j]) * llrs[j];
        }

        if (metric > best_metric) {
            best_metric = metric;
            best_word = candidate;
        }
    }

    return best_word;
}

} // namespace qpsk
