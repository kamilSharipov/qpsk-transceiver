#pragma once

#include "abstarct_decoder.hpp"
#include "encoder.hpp"

namespace qpsk {

#ifdef __AVX2__

template <int N>
class SimdDecoder : public AbstractDecoder<N> {
public:
    SimdDecoder();
    std::bitset<N> decode(const std::vector<double>& llrs) const override;
    std::string name() const override { return "SIMD"; }

private:
    std::array<std::array<double, CODEWORD_SIZE>, 1ULL << N> masks_;
};

#else

#warning "SIMD decoder not available"

template <int N>
class SimdDecoder : public AbstractDecoder {
private:
    SimdDecoder() = delete;
};

#endif

} // namespace qpsk
