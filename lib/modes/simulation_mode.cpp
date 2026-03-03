#include "system.hpp"
#include "encoder.hpp"
#include "qpsk.hpp"
#include "channel.hpp"
#include "random_bits.hpp"
#include "precomputed_decoder.hpp"

#include <iostream>

namespace qpsk {

template<int N>
int process_simulation(int iterations, double snr_db) {
    BlockEncoder<N> code;

    PrecomputedDecoder<N> decoder;
    QPSK mod;
    Channel channel(snr_db);
    
    int success = 0;
    for (int i = 0; i < iterations; ++i) {
        auto tx_bits = generate_random_bits<N>();

        auto cw = code.encode(tx_bits);
        auto symbols = mod.modulate(cw);

        auto rx_symbols = channel.apply(symbols);

        auto llrs = mod.demodulate(rx_symbols);
        auto rx_bits = decoder.decode(llrs);

        if (tx_bits == rx_bits) {
            ++success;
        }
    }
    return success;
}

int run_simulation_mode(const json& input, json& output) {
    if (!input.contains("num_of_pucch_f2_bits") ||
        !input.contains("iterations")) {
        std::cerr << "Error: missing fields for channel simulation\n";
        return 1;
    }

    const int n = input["num_of_pucch_f2_bits"];
    const int iterations = input["iterations"];
    const double snr_db = input.value("snr_db", 10.0);

    if (!input["iterations"].is_number_integer() || iterations <= 0) {
        std::cerr << "Error: 'iterations' must be positive integer\n";
        return 1;
    }

    int success = 0;

    try {
        switch (n) {
            case 2:  success = process_simulation<2>(iterations, snr_db); break;
            case 4:  success = process_simulation<4>(iterations, snr_db); break;
            case 6:  success = process_simulation<6>(iterations, snr_db); break;
            case 8:  success = process_simulation<8>(iterations, snr_db); break;
            case 11: success = process_simulation<11>(iterations, snr_db); break;
            default:
                throw std::invalid_argument("lib/modes/simulation_mode.cpp: invalid num_of_pucch_f2_bits");
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

} // namespace qpsk
