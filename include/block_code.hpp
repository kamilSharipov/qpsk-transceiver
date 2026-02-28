#pragma once

#include <vector>
#include <cstdint>

namespace qpsk {

const uint8_t base_matrix[20][13] = {
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1},
    {1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1},
    {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1},
    {1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1},
    {1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1},
    {1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1},
    {1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1},
    {1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1},
    {1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0}
};

class BlockCode {
public:
    explicit BlockCode(int n);

    std::vector<uint8_t> encode(const std::vector<bool>& info_bits) const;
    std::vector<bool> decode(const std::vector<double>& llrs) const;

private:
    int n_;
    std::vector<std::vector<uint8_t>> G_;

    void init_generator_matrix();
};

} // namespace qpsk
