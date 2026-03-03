#include <gtest/gtest.h>
#include <vector>
#include <bitset>

#include "encoder.hpp"
#include "basic_decoder.hpp"
#include "precomputed_decoder.hpp"
#include "simd_decoder.hpp"

using namespace qpsk;

template<int N, typename Decoder>
void test_decoder_no_noise(Decoder& decoder, const std::string& name) {
    BlockEncoder<N> encoder;
    const int total = 1 << N;
    
    for (int i = 0; i < total; ++i) {
        std::bitset<N> tx(i);
        auto cw = encoder.encode(tx);
        
        std::vector<double> llrs(CODEWORD_SIZE);
        for (size_t j = 0; j < CODEWORD_SIZE; ++j) {
            llrs[j] = cw[j] ? 10.0 : -10.0;
        }
        
        auto rx = decoder.decode(llrs);
        EXPECT_EQ(rx, tx) << name << " failed for input " << i;
    }
}

TEST(DecoderTest, BasicDecoderN2) {
    BasicDecoder<2> decoder;
    test_decoder_no_noise<2>(decoder, "BasicDecoder<2>");
}

TEST(DecoderTest, BasicDecoderN4) {
    BasicDecoder<4> decoder;
    test_decoder_no_noise<4>(decoder, "BasicDecoder<4>");
}

TEST(DecoderTest, BasicDecoderN6) {
    BasicDecoder<6> decoder;
    test_decoder_no_noise<6>(decoder, "BasicDecoder<6>");
}

TEST(DecoderTest, BasicDecoderN8) {
    BasicDecoder<8> decoder;
    test_decoder_no_noise<8>(decoder, "BasicDecoder<8>");
}

TEST(DecoderTest, BasicDecoderN11) {
    BasicDecoder<11> decoder;
    test_decoder_no_noise<11>(decoder, "BasicDecoder<11>");
}

TEST(DecoderTest, PrecomputedDecoderN2) {
    PrecomputedDecoder<2> decoder;
    test_decoder_no_noise<2>(decoder, "PrecomputedDecoder<2>");
}

TEST(DecoderTest, PrecomputedDecoderN4) {
    PrecomputedDecoder<4> decoder;
    test_decoder_no_noise<4>(decoder, "PrecomputedDecoder<4>");
}

TEST(DecoderTest, PrecomputedDecoderN6) {
    PrecomputedDecoder<6> decoder;
    test_decoder_no_noise<6>(decoder, "PrecomputedDecoder<6>");
}

TEST(DecoderTest, PrecomputedDecoderN8) {
    PrecomputedDecoder<8> decoder;
    test_decoder_no_noise<8>(decoder, "PrecomputedDecoder<8>");
}

TEST(DecoderTest, PrecomputedDecoderN11) {
    PrecomputedDecoder<11> decoder;
    test_decoder_no_noise<11>(decoder, "PrecomputedDecoder<11>");
}

#ifdef __AVX2__
TEST(DecoderTest, SimdDecoderN2) {
    SimdDecoder<2> decoder;
    test_decoder_no_noise<2>(decoder, "SimdDecoder<2>");
}

TEST(DecoderTest, SimdDecoderN4) {
    SimdDecoder<4> decoder;
    test_decoder_no_noise<4>(decoder, "SimdDecoder<4>");
}

TEST(DecoderTest, SimdDecoderN6) {
    SimdDecoder<6> decoder;
    test_decoder_no_noise<6>(decoder, "SimdDecoder<6>");
}

TEST(DecoderTest, SimdDecoderN8) {
    SimdDecoder<8> decoder;
    test_decoder_no_noise<8>(decoder, "SimdDecoder<8>");
}

TEST(DecoderTest, SimdDecoderN11) {
    SimdDecoder<11> decoder;
    test_decoder_no_noise<11>(decoder, "SimdDecoder<11>");
}
#endif

TEST(DecoderTest, AllDecodersSameResult) {
    BlockEncoder<4> encoder;
    std::bitset<4> tx("1010");
    auto cw = encoder.encode(tx);
    
    std::vector<double> llrs(CODEWORD_SIZE);
    for (size_t j = 0; j < CODEWORD_SIZE; ++j) {
        llrs[j] = cw[j] ? 2.0 : -2.0;
    }
    
    BasicDecoder<4> basic;
    PrecomputedDecoder<4> pre;
    
    auto res_basic = basic.decode(llrs);
    auto res_pre = pre.decode(llrs);
    
    EXPECT_EQ(res_basic, tx);
    EXPECT_EQ(res_pre, tx);
    
#ifdef __AVX2__
    SimdDecoder<4> simd;
    auto res_simd = simd.decode(llrs);
    EXPECT_EQ(res_simd, tx);
#endif
}

TEST(DecoderTest, InvalidLlrSize) {
    BasicDecoder<2> decoder;
    std::vector<double> llrs(19);
    
    EXPECT_THROW(decoder.decode(llrs), std::invalid_argument);
}

TEST(DecoderTest, WithNoise) {
    BlockEncoder<4> encoder;
    BasicDecoder<4> decoder;
    std::bitset<4> tx("1010");
    auto cw = encoder.encode(tx);
    
    std::vector<double> llrs(CODEWORD_SIZE);
    for (size_t j = 0; j < CODEWORD_SIZE; ++j) {
        double ideal = cw[j] ? 3.0 : -3.0;
        llrs[j] = ideal + (rand() % 100 - 50) / 100.0;
    }
    
    auto rx = decoder.decode(llrs);
    EXPECT_EQ(rx, tx);
}
