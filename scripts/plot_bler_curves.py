#!/usr/bin/env python3
# plot_bler_curves.py

import json
import subprocess
import numpy as np
import matplotlib.pyplot as plt
import os
import sys

SNR_VALUES = np.arange(-20, 10, 1.0)
CODE_SIZES = [2, 4, 6, 8, 11]
ITERATIONS = 1000
EXECUTABLE = "../build/qpsk"
TEMP_INPUT = "temp_input.json"
TEMP_OUTPUT = "result.json"

COLORS = ['blue', 'green', 'red', 'purple', 'orange']
MARKERS = ['o', 's', '^', 'D', 'v']


def run_simulation(n, snr_db, iterations):
    input_data = {
        "mode": "channel simulation",
        "num_of_pucch_f2_bits": n,
        "snr_db": float(snr_db),
        "iterations": iterations
    }

    with open(TEMP_INPUT, 'w') as f:
        json.dump(input_data, f, indent=2)

    try:
        result = subprocess.run(
            [EXECUTABLE, TEMP_INPUT],
            capture_output=True,
            text=True,
            timeout=60
        )

        if result.returncode != 0:
            print(f"  [n={n}, SNR={snr_db:.1f}] Error: {result.stderr.strip()}")
            return None

        with open(TEMP_OUTPUT, 'r') as f:
            output_data = json.load(f)

        return output_data['bler']

    except subprocess.TimeoutExpired:
        print(f"  [n={n}, SNR={snr_db:.1f}] Timeout")
        return None
    except Exception as e:
        print(f"  [n={n}, SNR={snr_db:.1f}] Exception: {e}")
        return None
    finally:
        if os.path.exists(TEMP_INPUT):
            os.remove(TEMP_INPUT)


def main():
    print("=" * 60)
    print("BLER vs SNR Simulation")
    print("=" * 60)
    print(f"SNR range: {SNR_VALUES[0]:.1f} to {SNR_VALUES[-1]:.1f} dB")
    print(f"Step: {SNR_VALUES[1] - SNR_VALUES[0]:.1f} dB")
    print(f"Code sizes: {CODE_SIZES}")
    print(f"Iterations per point: {ITERATIONS}")
    print("=" * 60)

    if not os.path.exists(EXECUTABLE):
        print(f"Error: executable {EXECUTABLE} not found")
        sys.exit(1)

    results = {n: [] for n in CODE_SIZES}
    total_points = len(CODE_SIZES) * len(SNR_VALUES)
    current_point = 0

    print(f"\nStarting simulation...")

    for n in CODE_SIZES:
        print(f"\nCode size: {n} bits")
        for snr in SNR_VALUES:
            current_point += 1
            # print(f"  [{current_point}/{total_points}] SNR = {snr:.1f} dB -> ", end="")

            bler = run_simulation(n, snr, ITERATIONS)

            if bler is not None:
                results[n].append(bler)
                # print(f"BLER = {bler:.6f}")
            else:
                results[n].append(1.0)
                print("FAILED")

    output_file = "bler_results.json"
    with open(output_file, 'w') as f:
        json.dump({
            "snr_values": SNR_VALUES.tolist(),
            "code_sizes": CODE_SIZES,
            "results": {str(n): results[n] for n in CODE_SIZES},
            "iterations": ITERATIONS
        }, f, indent=2)
    print(f"\nResults saved to {output_file}")

    print("\nGenerating plot...")
    plt.figure(figsize=(12, 8))

    for idx, n in enumerate(CODE_SIZES):
        if results[n]:
            plt.semilogy(
                SNR_VALUES[:len(results[n])],
                results[n],
                marker=MARKERS[idx],
                color=COLORS[idx],
                linewidth=2,
                markersize=6,
                markerfacecolor='white',
                markeredgewidth=2,
                label=f'n = {n} бит'
            )

    plt.grid(True, which="both", ls="-", alpha=0.2)
    plt.grid(True, which="major", ls="-", alpha=0.4)

    plt.xlabel('SNR (dB)', fontsize=14, fontweight='bold')
    plt.ylabel('BLER', fontsize=14, fontweight='bold')
    plt.title(f'Зависимость BLER от SNR для разных размеров кода\n(итераций: {ITERATIONS})',
              fontsize=16, fontweight='bold')

    plt.legend(fontsize=12, loc='upper right')

    plt.text(0.02, 0.98, f'Итераций: {ITERATIONS}',
             transform=plt.gca().transAxes, fontsize=10,
             verticalalignment='top',
             bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

    plt.xlim(min(SNR_VALUES), max(SNR_VALUES))
    plt.ylim(bottom=1e-6, top=1)
    plt.yscale('linear')
    plt.gca().yaxis.set_minor_formatter(plt.NullFormatter())
    plt.tight_layout()

    plt.savefig('bler_curves.png', dpi=300, bbox_inches='tight')
    plt.show()


if __name__ == "__main__":
    main()
