#include "utils/json_helpers.hpp"

namespace qpsk {

Complex parse_complex(const std::string& s) {
    size_t plus = s.find('+', 1);
    size_t minus = s.find('-', 1);
    size_t pos = std::string::npos;

    if (plus != std::string::npos && minus != std::string::npos) {
        pos = (plus < minus) ? plus : minus;
    } else if (plus != std::string::npos) {
        pos = plus;
    } else {
        pos = minus;
    }

    if (pos == std::string::npos || s.back() != 'j') {
        throw std::invalid_argument("lib/utils/json_helpers.cpp: invalid complex format: expected 'a+bj'");
    }

    std::string re_str = s.substr(0, pos);
    std::string im_str = s.substr(pos, s.size() - pos - 1);

    double re = std::stod(re_str);
    double im = std::stod(im_str);

    return Complex(re, im);
}

std::string format_complex(const Complex& c) {
    const std::string re = std::to_string(c.real());
    const std::string im = std::to_string(c.imag());

    if (c.imag() >= 0) {
        return re + "+" + im + "j";
    } else {
        return re + im + "j";
    }
}

} // namespace qpsk
