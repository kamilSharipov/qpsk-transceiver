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

template <int N>
class SimdDecoder : public AbstractDecoder<N> {
public:
    SimdDecoder() {
        static_assert(sizeof(N) == 0, "SIMD decoder requires AVX2 support");
    }

    std::bitset<N> decode(const std::vector<double>&) const override {
        throw std::runtime_error("SIMD decoder not available");
    }

    std::string name() const override { return "SIMD (unavailable)"; }
};

#endif

} // namespace qpsk
