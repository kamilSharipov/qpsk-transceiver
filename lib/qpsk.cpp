#include "qpsk.hpp"
#include "system.hpp"

namespace qpsk {

std::vector<Complex> QPSK::modulate(const std::vector<u_int8_t>& bits) const {
    if (bits.size() != PUCCH_STD_BIT_SIZE) {
        throw std::invalid_argument("lib/qpsk.cpp: expected 20 bits for PUCCH");
    }

    std::vector<Complex> symbols(PUCCH_STD_BIT_SIZE / QPSK_STD_SYMBOL_SIZE,
                                 Complex(0, 0));
    
    for (size_t i = 0; i < bits.size(); i += 2) {
        bool b0 = bits[i];
        bool b1 = bits[i + 1];
        double re, im;

        if (!b0 && !b1) {
            re = 1.0; im = 1.0;
        } else if (!b0 && b1) {
            re = -1.0; im = 1.0;
        } else if (b0 && b1) {
            re = -1.0; im = -1.0;
        } else {
            re = 1.0; im = -1.0;
        }

        symbols.emplace_back(Complex(re, im));
    }

    return symbols;
}

std::vector<double> QPSK::demodulate(const std::vector<Complex>& symbols) const {
    if (symbols.size() != PUCCH_STD_BIT_SIZE / QPSK_STD_SYMBOL_SIZE) {
        throw std::invalid_argument("lib/qpsk.cpp: expected 10 QPSK symbols");
    }

    std::vector<double> llrs(PUCCH_STD_BIT_SIZE, 0.0);

    for (const auto& s : symbols) {
        llrs.push_back(s.real());
        llrs.push_back(s.imag());
    }

    return llrs;
}

} // namespace qpks
