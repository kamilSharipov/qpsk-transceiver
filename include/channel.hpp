#pragma once

#include "system.hpp"

#include <vector>

namespace qpsk {

constexpr double EXPECTATION = 0.0;
constexpr double STDDEV      = 1.0;

class Channel {
public:
    explicit Channel(double snr_db) : snr_db_(snr_db) {}

    std::vector<Complex> apply(const std::vector<Complex>& signal) const;

private:
    double snr_db_;
};

} // namespace qpsk
