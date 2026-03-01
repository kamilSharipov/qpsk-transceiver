#include <gtest/gtest.h>

#include <vector>
#include <numeric>
#include <cmath>

#include "channel.hpp"

using namespace qpsk;

TEST(ChannelTest, EmptySignal) {
    Channel channel(10.0);
    std::vector<Complex> empty;
    EXPECT_THROW(channel.apply(empty), std::invalid_argument);
}

TEST(ChannelTest, NoisePowerMatchesSNR) {
    const double snr_db = 0.0;
    Channel channel(snr_db);

    std::vector<Complex> signal(1000, Complex(1.0, 0.0));

    auto noisy = channel.apply(signal);

    double noise_power = 0.0;
    for (size_t i = 0; i < signal.size(); ++i) {
        Complex noise = noisy[i] - signal[i];
        noise_power += std::norm(noise);
    }
    noise_power /= signal.size();

    EXPECT_NEAR(noise_power, 1.0, 0.1);
}

TEST(ChannelTest, HighSNRProducesLowNoise) {
    Channel channel(30.0);

    std::vector<Complex> signal(100, Complex(1.0, 1.0));
    auto noisy = channel.apply(signal);

    double max_deviation = 0.0;
    for (size_t i = 0; i < signal.size(); ++i) {
        double dev = std::abs(noisy[i].real() - signal[i].real()) +
                     std::abs(noisy[i].imag() - signal[i].imag());
        if (dev > max_deviation) max_deviation = dev;
    }

    EXPECT_LT(max_deviation, 0.2);
}

TEST(ChannelTest, LowSNRProducesHighNoise) {
    Channel channel(-5.0);

    std::vector<Complex> signal(100, Complex(1.0, 0.0));
    auto noisy = channel.apply(signal);

    double avg_abs_noise = 0.0;
    for (size_t i = 0; i < signal.size(); ++i) {
        Complex noise = noisy[i] - signal[i];
        avg_abs_noise += std::abs(noise.real()) + std::abs(noise.imag());
    }
    avg_abs_noise /= (2 * signal.size());

    EXPECT_GT(avg_abs_noise, 0.5);
}

TEST(ChannelTest, OutputSizeMatchesInput) {
    Channel channel(10.0);
    std::vector<Complex> signal(50, Complex(0.5, -0.5));
    auto noisy = channel.apply(signal);
    EXPECT_EQ(noisy.size(), signal.size());
}
