#include "system.hpp"
#include "encoder.hpp"
#include "qpsk.hpp"
#include "json_helpers.hpp"

#include <iostream>

namespace qpsk {

template<int N>
std::vector<Complex> process_coding(const json& bits_json) {
    BlockEncoder<N> code;
    std::bitset<N> info_bits;
    
    for (int i = 0; i < N; ++i) {
        if (!bits_json[i].is_number_integer()) {
            throw std::invalid_argument("lib/modes/coding_mode.cpp: each bit must be integer 0 or 1");
        }

        int bit_value = bits_json[i].get<int>();

        if (bit_value != 0 && bit_value != 1) {
            throw std::invalid_argument("lib/modes/coding_mode.cpp: bit value must be 0 or 1");
        }

        info_bits[i] = (bit_value == 1);
    }
    
    auto codeword = code.encode(info_bits);
    QPSK mod;
    return mod.modulate(codeword);
}

int run_coding_mode(const json& input, json& output) {
    if (!input.contains("num_of_pucch_f2_bits") || !input.contains("pucch_f2_bits")) {
        std::cerr << "Error: missing 'num_of_pucch_f2_bits' or 'pucch_f2_bits'\n";
        return 1;
    }

    const int n = input["num_of_pucch_f2_bits"];
    const auto bits_json = input["pucch_f2_bits"];

    if (!bits_json.is_array() || static_cast<int>(bits_json.size()) != n) {
        std::cerr << "Error: pucch_f2_bits must be array of " << n << " integers\n";
        return 1;
    }

    for (const auto& val : bits_json) {
        if (!val.is_number_integer()) {
            std::cerr << "Error: each bit must be integer 0 or 1\n";
            return 1;
        }
        int bit = val.get<int>();
        if (bit != 0 && bit != 1) {
            std::cerr << "Error: bit value must be 0 or 1, got " << bit << "\n";
            return 1;
        }
    }

    try {
        std::vector<Complex> symbols;

        switch (n) {
            case 2:  symbols = process_coding<2>(bits_json); break;
            case 4:  symbols = process_coding<4>(bits_json); break;
            case 6:  symbols = process_coding<6>(bits_json); break;
            case 8:  symbols = process_coding<8>(bits_json); break;
            case 11: symbols = process_coding<11>(bits_json); break;
            default:
                throw std::invalid_argument("lib/modes/coding_mode.cpp: invalid num_of_pucch_f2_bits");
        }

        json symbols_array = json::array();
        for (const auto& s : symbols) {
            symbols_array.push_back(format_complex(s));
        }

        output["mode"] = "coding";
        output["qpsk_symbols"] = symbols_array;
    } catch (const std::exception& e) {
        std::cerr << "Error during coding: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

} // namespace qpsk
