#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"; cd "$ROOT"
mkdir -p build
python tools/bin_to_hex.py --bin programs/sample.bin --data programs/sample.data --out-dir .
iverilog -g2012 -Wall -s tb_alu -o build/tb_alu rtl/sac16_alu.v tb/tb_alu.v
vvp build/tb_alu
iverilog -g2012 -Wall -s tb_sac16 -o build/tb_sac16 rtl/sac16_alu.v rtl/sac16_regfile.v rtl/sac16_imem.v rtl/sac16_dmem.v rtl/sac16_cpu.v tb/tb_sac16.v
vvp build/tb_sac16
if command -v gtkwave >/dev/null; then gtkwave build/sac16.vcd; else echo 'GTKWave missing; waveform at build/sac16.vcd'; fi
