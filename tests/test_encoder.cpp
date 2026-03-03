#include <gtest/gtest.h>
#include <vector>

#include "encoder.hpp"

using namespace qpsk;

TEST(BlockCodeTest, EncodeN2) {
    BlockEncoder<2> code;
    std::bitset<2> info("10");
    auto cw = code.encode(info);

    EXPECT_EQ(cw[0], 1);
    EXPECT_EQ(cw[1], 1);
}

TEST(BlockCodeTest, AllValidNWork) {
    auto test_n = [](int n) {
        if (n == 2) {
            BlockEncoder<2> c; std::bitset<2> i(1); c.encode(i);
        } else if (n == 4) {
            BlockEncoder<4> c; std::bitset<4> i(5); c.encode(i);
        } else if (n == 6) {
            BlockEncoder<6> c; std::bitset<6> i(21); c.encode(i);
        } else if (n == 8) {
            BlockEncoder<8> c; std::bitset<8> i(127); c.encode(i);
        } else if (n == 11) {
            BlockEncoder<11> c; std::bitset<11> i(1023); c.encode(i);
        }
    };

    for (int n : {2, 4, 6, 8, 11}) {
        EXPECT_NO_THROW(test_n(n)) << "Failed for N=" << n;
    }
}

TEST(BlockCodeTest, CodewordSizeIs20) {
    BlockEncoder<2> code;
    std::bitset<2> info("10");
    auto cw = code.encode(info);

    EXPECT_EQ(cw.size(), CODEWORD_SIZE);
}

TEST(BlockCodeTest, ZeroInputGivesZeroOutput) {
    BlockEncoder<11> code;
    std::bitset<11> zero_info(0);
    auto cw = code.encode(zero_info);

    for (size_t i = 0; i < CODEWORD_SIZE; ++i) {
        EXPECT_EQ(cw[i], 0) << "Non-zero bit at position " << i;
    }
}

TEST(BlockCodeTest, CorrectWordHasMaxMetric) {
    BlockEncoder<2> code;
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
