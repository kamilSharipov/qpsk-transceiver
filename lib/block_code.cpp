#include "block_code.hpp"
#include "system.hpp"

#include <stdexcept>
#include <vector>
#include <cmath>
#include <iostream>

namespace qpsk {

template class BlockCode<2>;
template class BlockCode<4>;
template class BlockCode<6>;
template class BlockCode<8>;
template class BlockCode<11>;

} // namespace qpsk
