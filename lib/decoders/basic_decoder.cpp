#include "basic_decoder.hpp"

namespace qpsk {

template <int N>
std::bitset<N> BasicDecoder<N>::decode(const std::vector<double>& llrs) const {
    if (llrs.size() != CODEWORD_SIZE) {
        throw std::invalid_argument("lib/decoders/basic_decoder.cpp: LLR vector must have 20 elements");
    }

    BlockEncoder<N> code;

    size_t total = 1ULL << N;
    double best_metric = -1e300;

    std::bitset<N> best_word;

    for (size_t i = 0; i < total; ++i) {
        std::bitset<N> candidate(i);
        std::bitset<CODEWORD_SIZE> codeword = code.encode(candidate);
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

template class BasicDecoder<2>;
template class BasicDecoder<4>;
template class BasicDecoder<6>;
template class BasicDecoder<8>;
template class BasicDecoder<11>;

} // namespace qpsk
