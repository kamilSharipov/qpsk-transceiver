#include "system.hpp"

#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.json>\n";
        return 1;
    }

    std::ifstream ifs(argv[1]);
    if (!ifs.is_open()) {
        std::cerr << "Cannot open input file\n";
        return 1;
    }

    json input;
    try {
        ifs >> input;
    } catch (const std::exception& e) {
        std::cerr << "Invalid JSON: " << e.what() << "\n";
        return 1;
    }

    json output;
    std::string mode = input.value("mode", "");

    int result = 1;
    if (mode == "coding") {
        result = run_coding_mode(input, output);
    } else if (mode == "decoding") {
        result = run_decoding_mode(input, output);
    } else if (mode == "channel simulation") {
        result = run_simulation_mode(input, output);
    } else {
        std::cerr << "Invalid mode\n";
        return 1;
    }

    if (result != 0) {
        return result;
    }

    std::ofstream ofs("result.json");
    ofs << output.dump(2) << "\n";

    return 0;
}
