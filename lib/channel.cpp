#include "channel.hpp"

#include <random>
#include <cmath>

namespace qpsk {

std::vector<Complex> Channel::apply(const std::vector<Complex>& signal) const {
    if (signal.empty()) {
        throw std::invalid_argument("sdffsdf");
    }

    std::vector<Complex> noisy = signal;

    double signal_power = 0.0;
    for (const auto& s : signal) {
        signal_power += std::norm(s);
    }
    signal_power /= signal.size();

    double noise_power = signal_power / std::pow(10.0, snr_db_ / 10.0);
    double sigma = std::sqrt(noise_power / 2.0);

    std::normal_distribution<double> dist(EXPECTATION, STDDEV);
    std::random_device rd;
    std::seed_seq seed{rd(), rd(), rd(), rd()};
    std::mt19937 gen(seed);

    for (auto& s: noisy) {
        double re_noise = sigma * dist(gen);
        double im_noise = sigma * dist(gen);
        s += Complex(re_noise, im_noise);
    }

    return noisy;
}

} // namespace qpsk
