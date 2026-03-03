#pragma once

#include "abstarct_decoder.hpp"
#include "encoder.hpp"

namespace qpsk {

template <int N>
class BasicDecoder : public AbstractDecoder<N> {
public:
    std::bitset<N> decode(const std::vector<double>& llrs) const override;
    std::string name() const override { return "Basic"; }
};

} // namespace qpsk
