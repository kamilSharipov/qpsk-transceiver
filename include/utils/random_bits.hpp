#pragma once

#include <bitset>
#include <random>

namespace qpsk {

template <int N>
std::bitset<N> generate_random_bits() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dis(0, 1);

    std::bitset<N> bits;

    for (int i = 0; i < N; ++i) {
        bits[i] = dis(rng) == 1;
    }

    return bits;
}

} // namespace qpsk
