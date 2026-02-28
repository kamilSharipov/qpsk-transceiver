#include "system.hpp"
#include "block_code.hpp"
#include "qpsk.hpp"
#include "channel.hpp"

#include <iostream>
#include <random>
#include <string>

using json = nlohmann::json;

qpsk::Complex parse_complex(const std::string& s) {
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
        throw std::invalid_argument("src/system.cpp: invalid complex format: expected 'a+bj'");
    }

    std::string re_str = s.substr(0, pos);
    std::string im_str = s.substr(pos, s.size() - pos - 1);

    double re = std::stod(re_str);
    double im = std::stod(im_str);

    return qpsk::Complex(re, im);
}

std::string format_complex(const qpsk::Complex& c) {
    const std::string re = std::to_string(c.real());
    const std::string im = std::to_string(c.imag());

    if (c.imag() >= 0) {
        return re + "+" + im + "j";
    } else {
        return re + im + "j";
    }
}

std::vector<bool> generate_random_bits(int n) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dis(0, 1);
    std::vector<bool> bits(n);

    for (int i = 0; i < n; ++i) {
        bits[i] = dis(rng) == 1;
    }
    return bits;
}
