#pragma once

#include "system.hpp"

#include <vector>

namespace qpsk {

class Channel {
public:
    explicit Channel(double snr_db) : snr_db_(snr_db) {}

    std::vector<Complex> apply(const std::vector<Complex>& signal) const;

private:
    static constexpr double GAUSSIAN_MEAN    = 0.0;
    static constexpr double GAUSSIAN_STD_DEV = 1.0;

    double snr_db_;
};

} // namespace qpsk
