#include "system.hpp"
#include "qpsk.hpp"
#include "json_helpers.hpp"
#include "precomputed_decoder.hpp"

#include <iostream>

namespace qpsk {

template<int N>
json process_decoding(const std::vector<double>& llrs) {
    PrecomputedDecoder<N> decoder;

    auto decoded = decoder.decode(llrs);

    json bits_array = json::array();

    for (int i = 0; i < N; ++i) {
        bits_array.push_back(decoded[i] ? 1 : 0);
    }

    return bits_array;
}

int run_decoding_mode(const json& input, json& output) {
    if (!input.contains("num_of_pucch_f2_bits") || !input.contains("qpsk_symbols")) {
        std::cerr << "Error: missing 'num_of_pucch_f2_bits' or 'qpsk_symbols'\n";
        return 1;
    }

    const int n = input["num_of_pucch_f2_bits"];
    const auto sym_json = input["qpsk_symbols"];

    if (!sym_json.is_array() || 
         sym_json.size() != qpsk::CODEWORD_SIZE / qpsk::QPSK_STD_SYMBOL_SIZE) {
        std::cerr << "Error: qpsk_symbols must be array of 10 strings like 'a+bj'\n";
        return 1;
    }

    std::vector<Complex> symbols;

    try {
        for (const auto& s_val : sym_json) {
            if (!s_val.is_string()) {
                std::cerr << "Error: each symbol must be a string\n";
                return 1;
            }
            symbols.push_back(parse_complex(s_val.get<std::string>()));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing symbol: " << e.what() << "\n";
        return 1;
    }

    try {
        QPSK mod;
        auto llrs = mod.demodulate(symbols);

        json bits_array;

        switch (n) {
            case 2:  bits_array = process_decoding<2>(llrs); break;
            case 4:  bits_array = process_decoding<4>(llrs); break;
            case 6:  bits_array = process_decoding<6>(llrs); break;
            case 8:  bits_array = process_decoding<8>(llrs); break;
            case 11: bits_array = process_decoding<11>(llrs); break;
            default:
                throw std::invalid_argument("lib/modes/decoding_mode.cpp: invalid num_of_pucch_f2_bits");
        }

        output["mode"] = "decoding";
        output["num_of_pucch_f2_bits"] = n;
        output["pucch_f2_bits"] = bits_array;
    } catch (const std::exception& e) {
        std::cerr << "Error during decoding: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

} // namespace qpsk
