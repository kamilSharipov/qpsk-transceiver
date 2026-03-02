#pragma once

#include "abstarct_decoder.hpp"
#include "encoder.hpp"

#include <array>

namespace qpsk {

template <int N>
class PrecomputedDecoder : public AbstractDecocer<N> {
public:
    PrecomputedDecoder();
    std::bitset<N> decode(const std::vector<double>& llrs) const override;
    std::string name() const override { return "Precomputed"; }

private:
    std::array<std::bitset<CODEWORD_SIZE>, 1ULL << N> codewords_;
};

} // namespace qpsk
