#include <gtest/gtest.h>
#include <string>
#include <complex>

#include "utils/json_helpers.hpp"

using namespace qpsk;

TEST(JsonHelpersTest, ParseComplexBasic) {
    auto c = parse_complex("1.5+2.5j");
    EXPECT_DOUBLE_EQ(c.real(), 1.5);
    EXPECT_DOUBLE_EQ(c.imag(), 2.5);
}

TEST(JsonHelpersTest, ParseComplexNegative) {
    auto c = parse_complex("-1.5-2.5j");
    EXPECT_DOUBLE_EQ(c.real(), -1.5);
    EXPECT_DOUBLE_EQ(c.imag(), -2.5);
}

TEST(JsonHelpersTest, ParseComplexMixed) {
    auto c = parse_complex("3.0-4.0j");
    EXPECT_DOUBLE_EQ(c.real(), 3.0);
    EXPECT_DOUBLE_EQ(c.imag(), -4.0);
    
    c = parse_complex("-3.0+4.0j");
    EXPECT_DOUBLE_EQ(c.real(), -3.0);
    EXPECT_DOUBLE_EQ(c.imag(), 4.0);
}

TEST(JsonHelpersTest, ParseComplexZero) {
    auto c = parse_complex("0+0j");
    EXPECT_DOUBLE_EQ(c.real(), 0.0);
    EXPECT_DOUBLE_EQ(c.imag(), 0.0);
}

TEST(JsonHelpersTest, ParseComplexInteger) {
    auto c = parse_complex("1+2j");
    EXPECT_DOUBLE_EQ(c.real(), 1.0);
    EXPECT_DOUBLE_EQ(c.imag(), 2.0);
}

TEST(JsonHelpersTest, ParseComplexInvalidFormat) {
    EXPECT_THROW(parse_complex("1.5+2.5"), std::invalid_argument);
    EXPECT_THROW(parse_complex("1.5+2.5i"), std::invalid_argument);
    EXPECT_THROW(parse_complex("abc"), std::invalid_argument);
    EXPECT_THROW(parse_complex("1.5++2.5j"), std::invalid_argument);
    EXPECT_THROW(parse_complex(""), std::invalid_argument);
}

TEST(JsonHelpersTest, FormatComplexBasic) {
    Complex c(1.5, 2.5);
    auto s = format_complex(c);
    EXPECT_EQ(s, "1.500000+2.500000j");
}

TEST(JsonHelpersTest, FormatComplexNegative) {
    Complex c(-1.5, -2.5);
    auto s = format_complex(c);
    EXPECT_EQ(s, "-1.500000-2.500000j");
}

TEST(JsonHelpersTest, FormatComplexMixed) {
    Complex c(3.0, -4.0);
    auto s = format_complex(c);
    EXPECT_EQ(s, "3.000000-4.000000j");

    c = Complex(-3.0, 4.0);
    s = format_complex(c);
    EXPECT_EQ(s, "-3.000000+4.000000j");
}

TEST(JsonHelpersTest, FormatComplexZero) {
    Complex c(0, 0);
    auto s = format_complex(c);
    EXPECT_EQ(s, "0.000000+0.000000j");
}

TEST(JsonHelpersTest, FormatComplexInteger) {
    Complex c(1, 2);
    auto s = format_complex(c);
    EXPECT_EQ(s, "1.000000+2.000000j");
}

TEST(JsonHelpersTest, ComplexRoundtrip) {
    std::vector<std::string> test_strings = {
        "1.5+2.5j",
        "-1.5-2.5j",
        "3.0-4.0j",
        "-3.0+4.0j",
        "0+0j",
        "1+2j"
    };

    for (const auto& s : test_strings) {
        auto c1 = parse_complex(s);
        auto s2 = format_complex(c1);
        auto c2 = parse_complex(s2);

        EXPECT_DOUBLE_EQ(c1.real(), c2.real());
        EXPECT_DOUBLE_EQ(c1.imag(), c2.imag());
    }
}
