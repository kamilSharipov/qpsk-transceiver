#include "encoder.hpp"

namespace qpsk {

template <int N>
std::bitset<CODEWORD_SIZE> BlockEncoder<N>::encode(const std::bitset<N>& info_bits) const {
    std::bitset<CODEWORD_SIZE> codeword;

    for (size_t i = 0; i < CODEWORD_SIZE; ++i) {
        bool bit = false;
        auto row = BASE_MATRIX[i];

        for (int j = 0; j < N; ++j) {
            if (info_bits[j] && row[j]) {
                bit = !bit;
            }
        }

        codeword[i] = bit;
    }

    return codeword;
}

template class BlockEncoder<2>;
template class BlockEncoder<4>;
template class BlockEncoder<6>;
template class BlockEncoder<8>;
template class BlockEncoder<11>;

} // namespace qpsk
