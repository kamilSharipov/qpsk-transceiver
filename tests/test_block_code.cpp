#include <gtest/gtest.h>
#include <vector>

#include "block_code.hpp"

using namespace qpsk;

TEST(BlockCodeTest, EncodeN2) {
    BlockCode<2> code;
    std::bitset<2> info("10");
    auto cw = code.encode(info);

    EXPECT_EQ(cw[0], 1);
    EXPECT_EQ(cw[1], 1);
}

TEST(BlockCodeTest, AllValidNWork) {
    auto test_n = [](int n) {
        if (n == 2) {
            BlockCode<2> c; std::bitset<2> i(1); c.encode(i);
        } else if (n == 4) {
            BlockCode<4> c; std::bitset<4> i(5); c.encode(i);
        } else if (n == 6) {
            BlockCode<6> c; std::bitset<6> i(21); c.encode(i);
        } else if (n == 8) {
            BlockCode<8> c; std::bitset<8> i(127); c.encode(i);
        } else if (n == 11) {
            BlockCode<11> c; std::bitset<11> i(1023); c.encode(i);
        }
    };

    for (int n : {2, 4, 6, 8, 11}) {
        EXPECT_NO_THROW(test_n(n)) << "Failed for N=" << n;
    }
}

TEST(BlockCodeTest, DecodeNoNoiseN2) {
    BlockCode<2> code;
    std::bitset<2> tx("10");
    auto cw = code.encode(tx);

    std::vector<double> llrs(CODEWORD_SIZE);
    for (size_t i = 0; i < CODEWORD_SIZE; ++i) {
        llrs[i] = cw[i] ? 1.0 : -1.0;
    }

    auto rx = code.decode(llrs);
    EXPECT_EQ(rx, tx);
}

TEST(BlockCodeTest, DecodeNoNoiseN4) {
    BlockCode<4> code;
    std::bitset<4> tx("1101");
    auto cw = code.encode(tx);

    std::vector<double> llrs(CODEWORD_SIZE);
    for (size_t i = 0; i < CODEWORD_SIZE; ++i) {
        llrs[i] = cw[i] ? 10.0 : -10.0;
    }

    auto rx = code.decode(llrs);
    EXPECT_EQ(rx, tx);
}

TEST(BlockCodeTest, DecodeInvalidLlrSize) {
    BlockCode<2> code;
    std::vector<double> llrs(19);

    EXPECT_THROW(code.decode(llrs), std::invalid_argument);
}

TEST(BlockCodeTest, CodewordSizeIs20) {
    BlockCode<2> code;
    std::bitset<2> info("10");
    auto cw = code.encode(info);

    EXPECT_EQ(cw.size(), CODEWORD_SIZE);
}

TEST(BlockCodeTest, ExhaustiveDecodeN2) {
    BlockCode<2> code;

    for (int i = 0; i < 4; ++i) {
        std::bitset<2> tx(i);
        auto cw = code.encode(tx);
        std::vector<double> llrs(CODEWORD_SIZE);

        for (size_t j = 0; j < CODEWORD_SIZE; ++j) {
            llrs[j] = cw[j] ? 5.0 : -5.0;
        }

        auto rx = code.decode(llrs);

        EXPECT_EQ(rx, tx) << "Failed for input " << i;
    }
}

TEST(BlockCodeTest, DecodeWithLowNoiseN4) {
    BlockCode<4> code;
    std::bitset<4> tx("1010");
    auto cw = code.encode(tx);

    std::vector<double> llrs(CODEWORD_SIZE);
    for (size_t i = 0; i < CODEWORD_SIZE; ++i) {
        double ideal_llr = cw[i] ? 1.0 : -1.0;
        // noise ~ N(0, 0.2)
        llrs[i] = ideal_llr + (ideal_llr > 0 ? -0.15 : 0.15);
    }

    auto rx = code.decode(llrs);
    EXPECT_EQ(rx, tx) << "Decoding failed under low noise";
}

TEST(BlockCodeTest, ZeroInputGivesZeroOutput) {
    BlockCode<11> code;
    std::bitset<11> zero_info(0);
    auto cw = code.encode(zero_info);

    for (size_t i = 0; i < CODEWORD_SIZE; ++i) {
        EXPECT_EQ(cw[i], 0) << "Non-zero bit at position " << i;
    }
}

TEST(BlockCodeTest, ExhaustiveDecodeN4) {
    BlockCode<4> code;
    const int total = 1 << 4;

    for (int i = 0; i < total; ++i) {
        std::bitset<4> tx(i);
        auto cw = code.encode(tx);
        std::vector<double> llrs(CODEWORD_SIZE);
        for (size_t j = 0; j < CODEWORD_SIZE; ++j) {
            llrs[j] = cw[j] ? 3.0 : -3.0;
        }
        auto rx = code.decode(llrs);
        EXPECT_EQ(rx, tx) << "Failed for input " << i;
    }
}

TEST(BlockCodeTest, CorrectWordHasMaxMetric) {
    BlockCode<2> code;
    std::bitset<2> tx("11");
    auto cw_tx = code.encode(tx);

    std::vector<double> llrs(CODEWORD_SIZE);
    for (size_t i = 0; i < CODEWORD_SIZE; ++i) {
        llrs[i] = cw_tx[i] ? 1.0 : -1.0;
    }

    double max_metric = -1e9;
    std::bitset<2> best;

    for (int i = 0; i < 4; ++i) {
        std::bitset<2> cand(i);
        auto cw = code.encode(cand);
        double metric = 0.0;

        for (size_t j = 0; j < CODEWORD_SIZE; ++j) {
            if (cw[j]) metric += llrs[j];
        }

        if (metric > max_metric) {
            max_metric = metric;
            best = cand;
        }
    }

    EXPECT_EQ(best, tx);
}
