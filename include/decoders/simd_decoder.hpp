#pragma once

#include "abstarct_decoder.hpp"
#include "encoder.hpp"

namespace qpsk {

template <int N>
class SimdDecoder : public AbstractDecocer<N> {
public:
    SimdDecoder();
    std::bitset<N> decode(const std::vector<double>& llrs) const override;
    std::string name() const override { return "SIMD"; }

private:
    std::array<std::array<double, CODEWORD_SIZE>, 1ULL << N> masks_;
};

} // namespace qpsk
