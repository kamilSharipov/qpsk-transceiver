#include "system.hpp"
#include "block_code.hpp"
#include "qpsk.hpp"
#include "channel.hpp"

#include <iostream>
#include <random>
#include <string>
#include <bitset>

qpsk::Complex parse_complex(const std::string& s) {
    size_t plus = s.find('+', 1);
    size_t minus = s.find('-', 1);
    size_t pos = std::string::npos;

    if (plus != std::string::npos && minus != std::string::npos) {
        pos = (plus < minus) ? plus : minus;
    } else if (plus != std::string::npos) {
        pos = plus;
    } else {
        pos = minus;
    }

    if (pos == std::string::npos || s.back() != 'j') {
        throw std::invalid_argument("src/system.cpp: invalid complex format: expected 'a+bj'");
    }

    std::string re_str = s.substr(0, pos);
    std::string im_str = s.substr(pos, s.size() - pos - 1);

    double re = std::stod(re_str);
    double im = std::stod(im_str);

    return qpsk::Complex(re, im);
}

std::string format_complex(const qpsk::Complex& c) {
    const std::string re = std::to_string(c.real());
    const std::string im = std::to_string(c.imag());

    if (c.imag() >= 0) {
        return re + "+" + im + "j";
    } else {
        return re + im + "j";
    }
}

template <int N>
std::bitset<N> generate_random_bits() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dis(0, 1);

    std::bitset<N> bits;

    for (int i = 0; i < N; ++i) {
        bits[i] = dis(rng) == 1;
    }

    return bits;
}

template<int N>
std::vector<qpsk::Complex> process_coding(const json& bits_json) {
    qpsk::BlockCode<N> code;
    std::bitset<N> info_bits;
    
    for (int i = 0; i < N; ++i) {
        info_bits[i] = bits_json[i].get<bool>();
    }
    
    auto codeword = code.encode(info_bits);
    qpsk::QPSK mod;
    return mod.modulate(codeword);
}

template<int N>
json process_decoding(const std::vector<double>& llrs) {
    qpsk::BlockCode<N> code;
    auto decoded = code.decode(llrs);
    
    json bits_array = json::array();

    for (int i = 0; i < N; ++i) {
        bits_array.push_back(static_cast<bool>(decoded[i]));
    }

    return bits_array;
}

template<int N>
int process_simulation(int iterations, double snr_db) {
    qpsk::BlockCode<N> code;
    qpsk::QPSK mod;
    qpsk::Channel channel(snr_db);
    
    int success = 0;
    for (int i = 0; i < iterations; ++i) {
        auto tx_bits = generate_random_bits<N>();

        auto cw = code.encode(tx_bits);
        auto symbols = mod.modulate(cw);

        auto rx_symbols = channel.apply(symbols);

        auto llrs = mod.demodulate(rx_symbols);
        auto rx_bits = code.decode(llrs);

        if (tx_bits == rx_bits) {
            ++success;
        }
    }
    return success;
}

int run_coding_mode(const json& input, json& output) {
    if (!input.contains("num_of_pucch_f2_bits") || !input.contains("pucch_f2_bits")) {
        std::cerr << "Error: missing 'num_of_pucch_f2_bits' or 'pucch_f2_bits'\n";
        return 1;
    }

    int n = input["num_of_pucch_f2_bits"];
    auto bits_json = input["pucch_f2_bits"];

    if (!bits_json.is_array() || static_cast<int>(bits_json.size()) != n) {
        std::cerr << "Error: pucch_f2_bits must be array of " << n << " booleans\n";
        return 1;
    }

    try {
        std::vector<qpsk::Complex> symbols;

        switch (n) {
            case 2:  symbols = process_coding<2>(bits_json); break;
            case 4:  symbols = process_coding<4>(bits_json); break;
            case 6:  symbols = process_coding<6>(bits_json); break;
            case 8:  symbols = process_coding<8>(bits_json); break;
            case 11: symbols = process_coding<11>(bits_json); break;
            default:
                throw std::invalid_argument("Invalid num_of_pucch_f2_bits");
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

int run_decoding_mode(const json& input, json& output) {
    if (!input.contains("num_of_pucch_f2_bits") || !input.contains("qpsk_symbols")) {
        std::cerr << "Error: missing 'num_of_pucch_f2_bits' or 'qpsk_symbols'\n";
        return 1;
    }

    int n = input["num_of_pucch_f2_bits"];
    auto sym_json = input["qpsk_symbols"];

    if (!sym_json.is_array() || 
         sym_json.size() != qpsk::CODEWORD_SIZE / qpsk::QPSK_STD_SYMBOL_SIZE) {
        std::cerr << "Error: qpsk_symbols must be array of 10 strings like 'a+bj'\n";
        return 1;
    }

    std::vector<qpsk::Complex> symbols;

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
        qpsk::QPSK mod;
        auto llrs = mod.demodulate(symbols);

        json bits_array = json::array();

        switch (n) {
            case 2:  bits_array = process_decoding<2>(llrs); break;
            case 4:  bits_array = process_decoding<4>(llrs); break;
            case 6:  bits_array = process_decoding<6>(llrs); break;
            case 8:  bits_array = process_decoding<8>(llrs); break;
            case 11: bits_array = process_decoding<11>(llrs); break;
            default:
                throw std::invalid_argument("Invalid num_of_pucch_f2_bits");
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

int run_simulation_mode(const json& input, json& output) {
    if (!input.contains("num_of_pucch_f2_bits") ||
        !input.contains("iterations")) {
        std::cerr << "Error: missing fields for channel simulation\n";
        return 1;
    }

    int n = input["num_of_pucch_f2_bits"];
    int iterations = input["iterations"];
    const int snr_db = 10;

    if (!input["iterations"].is_number_integer() || iterations <= 0) {
        std::cerr << "Error: 'iterations' must be positive integer\n";
        return 1;
    }

    int success;

    try {
        qpsk::QPSK mod;
        qpsk::Channel channel(snr_db);

        switch (n) {
            case 2:  success = process_simulation<2>(iterations, snr_db); break;
            case 4:  success = process_simulation<4>(iterations, snr_db); break;
            case 6:  success = process_simulation<6>(iterations, snr_db); break;
            case 8:  success = process_simulation<8>(iterations, snr_db); break;
            case 11: success = process_simulation<11>(iterations, snr_db); break;
            default:
                throw std::invalid_argument("Invalid num_of_pucch_f2_bits");
        }
    } catch (const std::exception& e) {
        std::cerr << "Simulation error: " << e.what() << "\n";
        return 1;
    }

    double bler = 1.0 - static_cast<double>(success) / iterations;

    output["mode"] = "channel simulation";
    output["num_of_pucch_f2_bits"] = n;
    output["bler"] = bler;
    output["success"] = success;
    output["failed"] = iterations - success;

    return 0;
}
