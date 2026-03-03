#pragma once

#include <vector>
#include <bitset>
#include <string>

namespace qpsk {

template <int N>
class AbstractDecoder {
public:
    virtual ~AbstractDecoder() = default;
    virtual std::bitset<N> decode(const std::vector<double>& llrs) const = 0;
    virtual std::string name() const = 0;
};

} // namespace qpsk
