# SAC16 Studio

A graphical frontend for the existing SAC16 project. The CPU and memory implementation
remain in C; the GUI uses a small C shared-library bridge and PySide6.

## Features

- Open and save `.asm` files.
- Assembly syntax highlighting.
- Click the left gutter beside a source line to toggle a breakpoint. Continue resumes past the breakpoint once.
- Source-line-to-instruction-address mapping for the current `.text` / `.data` / `.word`
  assembler syntax and labels.
- Run, Step, Continue, Stop, Reset.
- Live PC, SP, ACC, R0-R7, Zero and Carry display.
- Instruction Memory and Data Memory tables.
- Register and memory value-change watchpoints.
- Execution instruction count, modeled cycles, CPI and operation counters.
- Debug console.

## Install (Arch Linux)

```bash
sudo pacman -S --needed python python-pyside6 gcc
```

If PySide6 is not available as a system package:

```bash
python -m venv .venv
source .venv/bin/activate
pip install PySide6
```

## Build and run

From `SAC16/gui`:

```bash
python build_backend.py
python main.py
```

Open an assembly file and press **Build**. Then use Run / Step / Continue.
The first build compiles the existing assembler sources and simulator sources. It does
not modify the original CPU or assembler implementation.

## Important limitations of this first GUI version

- Source mapping supports the current assembler syntax: `.text`, labels, instructions,
  `.data`, `.word`, and `;` comments. If text-side address-changing directives are added
  later, the source mapper must be extended.
- Watchpoints detect value changes after each instruction, matching the current CLI
  behavior. They do not yet distinguish reads from writes.
- The Data Memory table initially displays the loaded range up to 256 words. The address
  lookup control can inspect any address in the 16-bit data memory.
- Cycle counts use the same estimates as the CLI (LD/ST=2, MUL=3, CALL/RET=2, otherwise 1).
- Breakpoints are toggled from the editor gutter and stop before the addressed instruction.
