#pragma once

#include <complex>
#include <string>

#include "nlohmann/json.hpp"

namespace qpsk {

constexpr size_t PUCCH_STD_BIT_SIZE = 20;

using Complex = std::complex<double>;

} // namespace qpsk

int run_coding_mode(const nlohmann::json& input, nlohmann::json& output);
int run_decoding_mode(const nlohmann::json& input, nlohmann::json& output);
int run_simulation_mode(const nlohmann::json& input, nlohmann::json& output);
