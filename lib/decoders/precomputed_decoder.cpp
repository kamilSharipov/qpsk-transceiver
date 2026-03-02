#include "precomputed_decoder.hpp"

namespace qpsk {

template <int N>
PrecomputedDecoder<N>::PrecomputedDecoder() {
    BlockEncoder<N> encoder;
    for (size_t i = 0; i < (1ULL << N); ++i) {
        codewords_[i] = encoder.encode(std::bitset<N>(i));
    }
}

template <int N>
typename std::bitset<N> PrecomputedDecoder<N>::decode(const std::vector<double>& llrs) const {
    if (llrs.size() != CODEWORD_SIZE) {
        throw std::invalid_argument("lib/decoders/precomputed_decoder.cpp: LLR vector must have 20 elements");
    }

    size_t total = 1ULL << N;
    double best_metric = -1e300;
    std::bitset<N> best_word;

    for (size_t i = 0; i < total; ++i) {
        const auto& codeword = codewords_[i];

        double metric = 0.0;
        for (size_t j = 0; j < CODEWORD_SIZE; ++j) {
            if (codeword[j]) {
                metric += llrs[j];
            }
        }

        if (metric > best_metric) {
            best_metric = metric;
            best_word = std::bitset<N>(i);
        }
    }

    return best_word;
}

template class PrecomputedDecoder<2>;
template class PrecomputedDecoder<4>;
template class PrecomputedDecoder<6>;
template class PrecomputedDecoder<8>;
template class PrecomputedDecoder<11>;

} // namespace qpsk
