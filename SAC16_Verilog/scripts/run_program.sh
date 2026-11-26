#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"; cd "$ROOT"
if [ "$#" -ne 2 ]; then echo "Usage: $0 path/to/program.bin path/to/program.data"; exit 2; fi
mkdir -p build
python tools/bin_to_hex.py --bin "$1" --data "$2" --out-dir .
iverilog -g2012 -Wall -s tb_sac16_run -o build/sac16_run rtl/sac16_alu.v rtl/sac16_regfile.v rtl/sac16_imem.v rtl/sac16_dmem.v rtl/sac16_cpu.v tb/tb_sac16_run.v
vvp build/sac16_run
if command -v gtkwave >/dev/null; then gtkwave build/program_run.vcd; else echo 'GTKWave missing; waveform at build/program_run.vcd'; fi
