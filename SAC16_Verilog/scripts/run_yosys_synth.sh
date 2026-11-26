#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"; cd "$ROOT"
command -v yosys >/dev/null || { echo 'Install: sudo pacman -S yosys'; exit 1; }
mkdir -p build
yosys -p 'read_verilog -sv rtl/sac16_alu.v rtl/sac16_regfile.v rtl/sac16_imem.v rtl/sac16_dmem.v rtl/sac16_cpu.v; hierarchy -check -top sac16_cpu; proc; opt; check; synth -top sac16_cpu; stat; write_verilog build/sac16_netlist.v' | tee build/yosys_synth.log
