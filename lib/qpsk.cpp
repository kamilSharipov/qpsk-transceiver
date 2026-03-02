#include "qpsk.hpp"
#include "system.hpp"

#include <iostream>

namespace qpsk {

std::vector<Complex> QPSK::modulate(const std::bitset<CODEWORD_SIZE>& bits) const {
    std::vector<Complex> symbols;
    symbols.reserve(QPSK_SYMBOLS_COUNT);
    
    for (size_t i = 0; i < CODEWORD_SIZE; i += 2) {
        bool b0 = bits[i];
        bool b1 = bits[i + 1];
        double re, im;

        if (!b0 && !b1) {
            re = -NORM;
            im = -NORM;
        } else if (!b0 && b1) {
            re = -NORM;
            im = NORM;
        } else if (b0 && b1) {
            re = NORM;
            im = NORM;
        } else {
            re = NORM;
            im = -NORM;
        }

        symbols.emplace_back(Complex(re, im));
    }

    return symbols;
}

std::vector<double> QPSK::demodulate(const std::vector<Complex>& symbols) const {
    if (symbols.size() != QPSK_SYMBOLS_COUNT) {
        throw std::invalid_argument("lib/qpsk.cpp: expected 10 QPSK symbols");
    }

    std::vector<double> llrs;
    llrs.reserve(CODEWORD_SIZE);

    for (const auto& s : symbols) {
        llrs.push_back(s.real());
        llrs.push_back(s.imag());
    }

    return llrs;
}

} // namespace qpsk
