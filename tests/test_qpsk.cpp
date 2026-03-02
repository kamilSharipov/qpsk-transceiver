#include <gtest/gtest.h>
#include <vector>
#include <bitset>

#include "qpsk.hpp"

using namespace qpsk;

TEST(QPSKTest, ModulateAllCombinations) {
    QPSK mod;

    std::vector<std::pair<std::bitset<2>, Complex>> cases = {
        {std::bitset<2>("00"), Complex(-NORM, -NORM)},
        {std::bitset<2>("01"), Complex(+NORM, -NORM)},
        {std::bitset<2>("11"), Complex( NORM,  NORM)},
        {std::bitset<2>("10"), Complex(-NORM, +NORM)}
    };

    for (const auto& [bits_pair, expected] : cases) {
        std::bitset<CODEWORD_SIZE> full_bits;
        full_bits[0] = bits_pair[0];
        full_bits[1] = bits_pair[1];

        auto symbols = mod.modulate(full_bits);

        ASSERT_EQ(symbols.size(), QPSK_SYMBOLS_COUNT);
        EXPECT_NEAR(symbols[0].real(), expected.real(), 1e-9)
            << "Failed for bits: " << bits_pair;
        EXPECT_NEAR(symbols[0].imag(), expected.imag(), 1e-9)
            << "Failed for bits: " << bits_pair;
    }
}

TEST(QPSKTest, EndToEndNoNoise) {
    QPSK mod;
    std::bitset<CODEWORD_SIZE> bits;

    for (size_t i = 0; i < CODEWORD_SIZE; ++i) {
        bits[i] = (i % 3 == 0);
    }

    auto symbols = mod.modulate(bits);
    auto llrs = mod.demodulate(symbols);

    for (size_t i = 0; i < CODEWORD_SIZE; ++i) {
        if (bits[i]) {
            EXPECT_GT(llrs[i], 0.0) << "Bit " << i << " is 1 but LLR <= 0";
        } else {
            EXPECT_LT(llrs[i], 0.0) << "Bit " << i << " is 0 but LLR >= 0";
        }
    }
}

TEST(QPSKTest, ModulateOutputSize) {
    QPSK mod;
    std::bitset<CODEWORD_SIZE> bits;
    auto symbols = mod.modulate(bits);

    EXPECT_EQ(symbols.size(), QPSK_SYMBOLS_COUNT);
}

TEST(QPSKTest, DemodulateOutputSize) {
    QPSK mod;
    std::vector<Complex> symbols(QPSK_SYMBOLS_COUNT, Complex(1.0, 1.0));
    auto llrs = mod.demodulate(symbols);

    EXPECT_EQ(llrs.size(), CODEWORD_SIZE);
}

TEST(QPSKTest, DemodulateInvalidInputSize) {
    QPSK mod;
    std::vector<Complex> symbols(9, Complex(1.0, 1.0));

    EXPECT_THROW(mod.demodulate(symbols), std::invalid_argument);
}

TEST(QPSKTest, IntegrationWithDecoder) {
    BlockCode<2> decoder;
    QPSK mod;

    std::bitset<2> info("10");
    auto cw = decoder.encode(info);

    auto symbols = mod.modulate(cw);
    auto llrs = mod.demodulate(symbols);

    auto recovered = decoder.decode(llrs);

    EXPECT_EQ(recovered, info);
}
