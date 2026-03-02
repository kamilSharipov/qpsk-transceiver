#include "encoder.hpp"
#include "basic_decoder.hpp"
#include "precomputed_decoder.hpp"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>
#include <vector>

using namespace qpsk;

std::vector<double> generate_random_llrs() {
    std::vector<double> llrs(CODEWORD_SIZE);
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution<double> dist(-10.0, 10.0);
    
    for (auto& llr : llrs) {
        llr = dist(rng);
    }
    return llrs;
}

template <typename Decoder, int N>
double benchmark_decoder(const Decoder& decoder, const std::vector<double>& llrs, 
                         size_t iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < iterations; ++i) {
        volatile auto result = decoder.decode(llrs);
        (void)result;
    }
    
    auto end = std::chrono::high_resolution_clock::now();

    return std::chrono::duration<double, std::milli>(end - start).count();
}

template <int N>
void run_benchmarks(size_t iterations) {
    std::cout << "\n========================================\n";
    std::cout << "Benchmark for N = " << N << " bits\n";
    std::cout << "Combinations: " << (1ULL << N) << "\n";
    std::cout << "Iterations: " << iterations << "\n";
    std::cout << "========================================\n";

    BasicDecoder<N> basic;
    PrecomputedDecoder<N> precomputed;

    auto llrs = generate_random_llrs();

    // Прогрев
    for (int i = 0; i < 100; ++i) {
        basic.decode(llrs);
        precomputed.decode(llrs);
    }

    std::cout << "\nResults:\n";
    std::cout << std::string(40, '-') << "\n";
    
    double time_basic = benchmark_decoder<BasicDecoder<N>, N>(basic, llrs, iterations);
    std::cout << "Basic:       " << std::fixed << std::setprecision(3)
              << std::setw(8) << time_basic << " ms\n";
    
    double time_pre = benchmark_decoder<PrecomputedDecoder<N>, N>(precomputed, llrs, iterations);
    std::cout << "Precomputed: " << std::setw(8) << time_pre << " ms"
              << "  (x" << std::setprecision(2) << (time_basic / time_pre) << ")\n";
}

int main() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "Decoder Benchmark\n";
    std::cout << "========================================\n\n";

    std::cout << "\n";
    run_benchmarks<2>(10000);
    run_benchmarks<4>(10000);
    run_benchmarks<6>(10000);
    run_benchmarks<8>(10000);
    run_benchmarks<11>(10000);

    return 0;
}
