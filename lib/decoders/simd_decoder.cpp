#include "simd_decoder.hpp"

#include <immintrin.h>

namespace qpsk {

template <int N>
SimdDecoder<N>::SimdDecoder() {
    BlockEncoder<N> encoder;
    
    for (size_t i = 0; i < (1ULL << N); i++) {
        auto codeword = encoder.encode(std::bitset<N>(i));

        for (size_t j = 0; j < CODEWORD_SIZE; ++j) {
            masks_[i][j] = codeword[j] ? 1.0 : 0.0;
        }
    }
    
}

template <int N>
std::bitset<N> SimdDecoder<N>::decode(const std::vector<double>& llrs) const {
    if (llrs.size() != CODEWORD_SIZE) {
        throw std::invalid_argument("lib/decoders/simd_decoder.cpp: LLR vector must have 20 elements");
    }

    size_t total = 1ULL << N;
    double best_metric = -1e300;
    std::bitset<N> best_word;

    __m256d sum;
    const double* llr_data = llrs.data();

    for (size_t i = 0; i < total; ++i) {
        sum = _mm256_setzero_pd();

        for (size_t j = 0; j < CODEWORD_SIZE; j += 4) {
            __m256d llr_vec = _mm256_loadu_pd(llr_data + j);
            __m256d mask_vec = _mm256_loadu_pd(&masks_[i][j]);
            sum = _mm256_add_pd(sum, _mm256_mul_pd(llr_vec, mask_vec));
        }

        double metric_array[4];
        _mm256_storeu_pd(metric_array, sum);
        double metric = metric_array[0] + metric_array[1] + 
                        metric_array[2] + metric_array[3];

        if (metric > best_metric) {
            best_metric = metric;
            best_word = std::bitset<N>(i);
        }
    }

    return best_word;
}

template class SimdDecoder<2>;
template class SimdDecoder<4>;
template class SimdDecoder<6>;
template class SimdDecoder<8>;
template class SimdDecoder<11>;

} // namespace qpsk
