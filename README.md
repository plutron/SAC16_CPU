# SAC16 — 16-bit CPU Architecture, Assembler & Simulator

<p align="center">
  <strong>A Custom 16-bit Processor, Assembler, Disassembler & Debugging Environment</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Language-C-blue?style=flat-square" />
  <img src="https://img.shields.io/badge/Hardware-Verilog-purple?style=flat-square" />
  <img src="https://img.shields.io/badge/Architecture-16--bit-informational?style=flat-square" />
  <img src="https://img.shields.io/badge/GUI-PySide6-green?style=flat-square" />
  <img src="https://img.shields.io/badge/Platform-Linux-lightgrey?style=flat-square" />
</p>

---

# فارسی

## معرفی پروژه

**SAC16** یک پروژه پیاده‌سازی و شبیه‌سازی یک معماری پردازنده اختصاصی ۱۶ بیتی است.

هدف پروژه، طراحی یک مجموعه ابزار کامل برای کار با یک ISA اختصاصی است؛ از نوشتن برنامه به زبان اسمبلی و تبدیل آن به کد ماشین گرفته تا اجرای دستورها، بررسی وضعیت داخلی پردازنده و دیباگ برنامه‌ها.

این پروژه شامل دو بخش اصلی است:

- پیاده‌سازی نرم‌افزاری پردازنده و ابزارهای مرتبط با زبان C
- پیاده‌سازی سخت‌افزاری معماری پردازنده با استفاده از Verilog

علاوه بر ابزارهای خط فرمان، یک محیط گرافیکی برای اجرای برنامه‌ها و بررسی عملکرد پردازنده نیز توسعه داده شده است.

## معماری SAC16

SAC16 یک پردازنده آموزشی ۱۶ بیتی با مجموعه دستورالعمل اختصاصی است.

مشخصات اصلی معماری:

| ویژگی | مشخصات |
|---|---|
| طول دستورالعمل | 16 بیت |
| Opcode | 4 بیت |
| Instruction Memory | 4096 کلمه |
| Data Memory | 65536 کلمه |
| General Purpose Registers | 8 عدد (R0–R7) |
| Accumulator | ACC |
| Program Counter | 12 بیت در پیاده‌سازی RTL |
| Stack Pointer | 16 بیت |
| Flags | Zero و Carry |
| Execution Model | چندسیکل (Multi-cycle) در RTL |

### مجموعه دستورالعمل‌ها (ISA)

مجموعه دستورالعمل SAC16 شامل ۱۶ دستور است:

| Opcode | Instruction | توضیح |
|---|---|---|
| 0x0 | LD | بارگذاری از حافظه |
| 0x1 | ST | ذخیره در حافظه |
| 0x2 | MOV | انتقال داده بین رجیسترها |
| 0x3 | ADD | جمع |
| 0x4 | SUB | تفریق |
| 0x5 | MUL | ضرب |
| 0x6 | AND | AND منطقی |
| 0x7 | OR | OR منطقی |
| 0x8 | NOT | معکوس‌سازی بیت‌ها |
| 0x9 | JMP | پرش بدون شرط |
| 0xA | JZ | پرش در صورت صفر بودن ACC |
| 0xB | JNZ | پرش در صورت صفر نبودن ACC |
| 0xC | CALL | فراخوانی تابع |
| 0xD | RET | بازگشت از تابع |
| 0xE | LDI | بارگذاری مقدار فوری |
| 0xF | HALT | توقف پردازنده |

## بخش‌های پروژه

### 1. Assembler

اسمبلر اختصاصی نوشته‌شده با زبان C که برنامه‌های اسمبلی SAC16 را به کد ماشین تبدیل می‌کند.

قابلیت‌ها:
- پردازش فایل اسمبلی به‌صورت خط‌به‌خط
- پشتیبانی از بخش‌های `.text` و `.data`
- مدیریت Labelها و Symbol Table
- پشتیبانی از دستور `.word`
- تشخیص و اعتبارسنجی Opcodeها و Operandها
- بررسی شماره رجیسترها و مقادیر Immediate
- مدیریت خطاهای نحوی و ساختاری
- تولید فایل‌های مجزای حافظه دستورالعمل و داده

برای دستورالعمل‌ها، قالب‌های R، N، Jump و LDI در نظر گرفته شده‌اند.

خروجی دستورالعمل‌ها و داده‌ها به‌صورت فایل‌های باینری ۱۶ بیتی با ترتیب ذخیره‌سازی Big Endian تولید می‌شود.

### 2. Disassembler

ابزاری برای تبدیل کد ماشین SAC16 به اسمبلی قابل خواندن.

قابلیت‌ها:
- خواندن فایل حافظه دستورالعمل
- استخراج Opcode و فیلدهای دستور
- تشخیص قالب دستورالعمل
- تبدیل شماره رجیسترها به نام‌های R0 تا R7 و ACC
- بازیابی اطلاعات حافظه داده
- تشخیص دستورهای نامعتبر
- تولید فایل اسمبلی با پسوند `.asm`

نکته: به دلیل حذف اطلاعاتی مانند نام Labelها و متغیرها در فرایند اسمبلی، خروجی Disassembler لزوماً نسخه دقیق فایل اسمبلی اولیه نیست.

### 3. CPU Simulator

شبیه‌ساز نرم‌افزاری پردازنده که با زبان C پیاده‌سازی شده است.

اجزای اصلی:
- CPU
- ALU
- Register File
- Instruction Memory
- Data Memory
- Decoder
- Datapath
- Control Unit
- Flags Management

قابلیت‌ها:
- اجرای دستورالعمل‌های SAC16
- مدیریت رجیسترها و حافظه‌ها
- اجرای دستورهای محاسباتی و منطقی
- مدیریت پرش‌ها و فراخوانی توابع
- مدیریت Stack
- بررسی اعتبار دستورها و مدیریت خطاها
- جمع‌آوری آمار اجرای برنامه

### 4. Debugging System

برای بررسی رفتار پردازنده و برنامه‌ها، امکانات دیباگ در نسخه خط فرمان و رابط گرافیکی در نظر گرفته شده است.

قابلیت‌ها:
- اجرای عادی برنامه
- اجرای مرحله‌به‌مرحله (Step)
- حالت Trace
- Breakpoint
- Watchpoint برای رجیسترها و حافظه
- مشاهده وضعیت رجیسترها
- بررسی تغییرات حافظه
- شمارش دستورهای اجراشده
- محاسبه Cycleهای مدل‌سازی‌شده و CPI

### 5. SAC16 Studio — Graphical Interface

یک رابط گرافیکی برای تعامل با اسمبلر و شبیه‌ساز که با Python و PySide6 توسعه داده شده است.

هسته پردازنده و منطق شبیه‌سازی همچنان در C قرار دارند و رابط گرافیکی از طریق یک کتابخانه واسط C با آن‌ها ارتباط برقرار می‌کند.

امکانات:

- ویرایشگر اسمبلی با Syntax Highlighting
- باز کردن و ذخیره فایل‌های اسمبلی
- ساخت برنامه و آماده‌سازی حافظه‌ها
- Run / Step / Continue / Stop / Reset
- تعیین Breakpoint از طریق ویرایشگر
- نمایش زنده PC، SP، ACC و R0–R7
- نمایش پرچم‌های Zero و Carry
- مشاهده Instruction Memory و Data Memory
- Debug Console
- Execution Statistics
- Register و Memory Watchpoints

### 6. Verilog Hardware Implementation

در کنار نسخه نرم‌افزاری، یک پیاده‌سازی RTL از معماری SAC16 با استفاده از Verilog توسعه داده شده است.

ماژول‌های اصلی:

- `sac16_alu.v` — واحد محاسباتی و منطقی
- `sac16_regfile.v` — فایل رجیسترها
- `sac16_imem.v` — حافظه دستورالعمل
- `sac16_dmem.v` — حافظه داده
- `sac16_cpu.v` — هسته اصلی پردازنده

در این پیاده‌سازی، کنترل اجرای دستورها با استفاده از یک ماشین حالت (FSM) انجام می‌شود.

مراحل اصلی اجرای دستور:
- Fetch
- Decode
- Execute
- Memory Read / Writeback (در صورت نیاز)
- Return Read / Writeback (برای RET)

برخی دستورها در سه سیکل و دستورهای نیازمند دسترسی به حافظه یا پشته در سیکل‌های بیشتری اجرا می‌شوند.

### 7. Verification & Simulation

برای شبیه‌سازی و بررسی پیاده‌سازی Verilog از ابزارهای زیر استفاده شده است:

- Icarus Verilog
- GTKWave

تست‌بنچ‌ها برای بررسی ALU و عملکرد CPU در نظر گرفته شده‌اند.

همچنین ابزار تبدیل فایل‌های باینری به فرمت Hex برای بارگذاری حافظه‌ها با `$readmemh` توسعه داده شده است.

اسکریپت‌های اجرای شبیه‌سازی، اجرای برنامه و سنتز با Yosys نیز در پروژه قرار دارند.

توجه: پیاده‌سازی RTL با Icarus Verilog شبیه‌سازی و شکل موج‌ها با GTKWave بررسی شده‌اند. اجرای پروژه روی Vivado در مستندات فعلی تأیید نشده است.

## ساختار کلی پروژه

```text
SAC16/
│
├── documents/
│   ├── assembler.html
│   ├── disassembler.html
│   └── simulator.html
│
├── SAC16_CPU-main/
│   ├── assembler/
│   │   └── compiler/
│   │       ├── include/
│   │       ├── src/
│   │       └── compiler.c
│   │
│   ├── disassembler/
│   │   └── disassembler.c
│   │
│   ├── simulator/
│   │   ├── include/
│   │   ├── src/
│   │   └── simulator.c
│   │
│   └── gui/
│       ├── bridge.c
│       ├── main.py
│       ├── window.py
│       └── build_backend.py
│
└── SAC16_Verilog_Arch/
    ├── rtl/
    ├── tb/
    ├── scripts/
    ├── tools/
    └── programs/
```

## راه‌اندازی و اجرا

### پیش‌نیازها

- GCC
- CMake
- Python 3
- PySide6 (برای رابط گرافیکی)
- Icarus Verilog (برای شبیه‌سازی RTL)
- GTKWave (برای مشاهده شکل موج‌ها)
- Yosys (برای اجرای اسکریپت سنتز)

### ساخت Assembler

```bash
cd SAC16_CPU-main/assembler/compiler

cmake -S . -B build
cmake --build build
```

فایل اجرایی در مسیر زیر ساخته می‌شود:

```text
bin/sac16
```

### ساخت Disassembler

```bash
cd SAC16_CPU-main/disassembler

gcc -std=c11 -Wall -Wextra -o disassembler disassembler.c
```

نمونه اجرا:

```bash
./disassembler -b instruction.bin
```

برای تبدیل هم‌زمان حافظه دستور و داده:

```bash
./disassembler -b instruction.bin -d data.bin
```

### اجرای رابط گرافیکی

پیش‌نیازهای اصلی در Arch Linux:

```bash
sudo pacman -S --needed python python-pyside6 gcc
```

سپس:

```bash
cd SAC16_CPU-main/gui

python build_backend.py
python main.py
```

### شبیه‌سازی نسخه Verilog

ابتدا Icarus Verilog و GTKWave را نصب کن.

برای اجرای یک برنامه:

```bash
cd SAC16_Verilog_Arch

./scripts/run_program.sh \
    programs/working/1.bin \
    programs/working/1.data
```

برای اجرای اسکریپت سنتز Yosys:

```bash
./scripts/run_yosys_synth.sh
```

## محدودیت‌ها و وضعیت فعلی

- پروژه در محیط Linux توسعه و تست شده است؛ اجرای آن در Windows ممکن است به کامپایل مجدد و تنظیمات متناسب نیاز داشته باشد.
- نسخه فعلی اسمبلر از یک بخش `.data` و یک بخش `.text` در هر فایل پشتیبانی می‌کند.
- برای تعریف داده از `.word` استفاده می‌شود.
- بازسازی نام اصلی Labelها و متغیرها در Disassembler امکان‌پذیر نیست.
- برخی قابلیت‌های پیشرفته‌تر اسمبلی و دیباگ هنوز قابل توسعه هستند.
- نسخه RTL هنوز روی Vivado اعتبارسنجی نشده است.

## مستندات

برای آشنایی بیشتر با پیاده‌سازی و جزئیات هر بخش، مستندات پروژه را در پوشه `documents` و فایل توضیحات معماری RTL را در پوشه `SAC16_Verilog_Arch` مشاهده کنید.

---

# English

## Overview

**SAC16** is a custom 16-bit processor architecture and simulation project.

The project aims to provide a complete toolchain for a custom Instruction Set Architecture (ISA), covering the entire workflow from writing assembly programs and generating machine code to executing instructions, inspecting CPU state, and debugging programs.

The project consists of two main implementations:

- A software-based CPU and toolchain implemented in C.
- A hardware-oriented RTL implementation of the CPU using Verilog.

A graphical environment is also provided to make program execution and processor inspection easier.

## Architecture

SAC16 is an educational 16-bit processor with a custom instruction set.

| Feature | Specification |
|---|---|
| Instruction Width | 16 bits |
| Opcode Width | 4 bits |
| Instruction Memory | 4096 words |
| Data Memory | 65536 words |
| General-Purpose Registers | 8 (R0–R7) |
| Accumulator | ACC |
| Program Counter | 12 bits in the RTL implementation |
| Stack Pointer | 16 bits |
| Flags | Zero and Carry |
| RTL Execution Model | Multi-cycle |

### Instruction Set Architecture (ISA)

SAC16 supports 16 instructions:

| Opcode | Instruction | Description |
|---|---|---|
| 0x0 | LD | Load from memory |
| 0x1 | ST | Store to memory |
| 0x2 | MOV | Register-to-register transfer |
| 0x3 | ADD | Addition |
| 0x4 | SUB | Subtraction |
| 0x5 | MUL | Multiplication |
| 0x6 | AND | Logical AND |
| 0x7 | OR | Logical OR |
| 0x8 | NOT | Bitwise inversion |
| 0x9 | JMP | Unconditional jump |
| 0xA | JZ | Jump if ACC is zero |
| 0xB | JNZ | Jump if ACC is not zero |
| 0xC | CALL | Function call |
| 0xD | RET | Function return |
| 0xE | LDI | Load immediate value |
| 0xF | HALT | Halt execution |

## Project Components

### 1. Assembler

A custom assembler implemented in C that translates SAC16 assembly programs into machine code.

Features:
- Line-by-line assembly source processing
- `.text` and `.data` section support
- Label handling and Symbol Table management
- `.word` data directives
- Opcode and operand validation
- Register and immediate value validation
- Syntax and structural error handling
- Separate instruction and data memory output files

The assembler supports R, N, Jump, and LDI instruction formats.

Machine code and data are generated as 16-bit binary words using Big Endian byte ordering.

### 2. Disassembler

A tool that translates SAC16 machine code back into readable assembly instructions.

Features:
- Instruction memory file parsing
- Opcode and instruction field extraction
- Instruction format recognition
- Register number-to-name conversion
- Data memory decoding
- Invalid instruction detection
- Assembly source generation

The disassembler cannot fully reconstruct the original assembly source because information such as label names, variable names, and comments is not preserved in machine code.

### 3. CPU Simulator

A software-based CPU simulator implemented in C.

Main components:
- CPU
- ALU
- Register File
- Instruction Memory
- Data Memory
- Decoder
- Datapath
- Control Unit
- Flag Management

Features:
- SAC16 instruction execution
- Register and memory management
- Arithmetic and logical operations
- Branch and function call handling
- Stack operations
- Instruction validation and error handling
- Execution statistics collection

### 4. Debugging System

Debugging capabilities are provided through both the command-line simulator and the graphical interface.

Features:
- Normal execution
- Step-by-step execution
- Trace mode
- Breakpoints
- Register and memory watchpoints
- Register inspection
- Memory change tracking
- Instruction execution statistics
- Modeled cycle counting and CPI calculation

### 5. SAC16 Studio — Graphical Interface

A graphical interface developed using Python and PySide6.

The CPU implementation and simulation logic remain in C. A small C shared-library bridge connects the graphical frontend to the existing simulator.

Features:
- Assembly editor with syntax highlighting
- Open and save assembly files
- Build and prepare programs for execution
- Run / Step / Continue / Stop / Reset
- Editor-based breakpoint management
- Live PC, SP, ACC, and R0–R7 monitoring
- Zero and Carry flag visualization
- Instruction Memory and Data Memory inspection
- Debug Console
- Execution Statistics
- Register and Memory Watchpoints

### 6. Verilog Hardware Implementation

An RTL implementation of the SAC16 architecture has also been developed using Verilog.

Main modules:

- `sac16_alu.v` — Arithmetic Logic Unit
- `sac16_regfile.v` — Register File
- `sac16_imem.v` — Instruction Memory
- `sac16_dmem.v` — Data Memory
- `sac16_cpu.v` — Main CPU Core

Instruction execution is controlled through a Finite State Machine (FSM).

Execution stages include:
- Fetch
- Decode
- Execute
- Memory Read / Writeback when required
- Return Read / Writeback for RET

Regular instructions take three clock cycles, while memory and stack-related instructions require additional cycles.

### 7. Verification & Simulation

The Verilog implementation has been simulated using:

- Icarus Verilog
- GTKWave

Testbenches are provided for the ALU and CPU.

A binary-to-hex conversion utility is also included to prepare memory initialization files for `$readmemh`.

The project includes scripts for simulation, program execution, and Yosys synthesis.

**Note:** The RTL implementation has been simulated using Icarus Verilog, and its waveforms have been inspected with GTKWave. Vivado validation has not been confirmed in the current project documentation.

## Project Structure

```text
SAC16/
│
├── documents/
│   ├── assembler.html
│   ├── disassembler.html
│   └── simulator.html
│
├── SAC16_CPU-main/
│   ├── assembler/
│   │   └── compiler/
│   │       ├── include/
│   │       ├── src/
│   │       └── compiler.c
│   │
│   ├── disassembler/
│   │   └── disassembler.c
│   │
│   ├── simulator/
│   │   ├── include/
│   │   ├── src/
│   │   └── simulator.c
│   │
│   └── gui/
│       ├── bridge.c
│       ├── main.py
│       ├── window.py
│       └── build_backend.py
│
└── SAC16_Verilog_Arch/
    ├── rtl/
    ├── tb/
    ├── scripts/
    ├── tools/
    └── programs/
```

## Build & Run

### Requirements

- GCC
- CMake
- Python 3
- PySide6 (for the GUI)
- Icarus Verilog (for RTL simulation)
- GTKWave (for waveform inspection)
- Yosys (for the synthesis script)

### Build the Assembler

```bash
cd SAC16_CPU-main/assembler/compiler

cmake -S . -B build
cmake --build build
```

The executable is generated at:

```text
bin/sac16
```

### Build the Disassembler

```bash
cd SAC16_CPU-main/disassembler

gcc -std=c11 -Wall -Wextra -o disassembler disassembler.c
```

Example:

```bash
./disassembler -b instruction.bin
```

To decode both instruction and data memory:

```bash
./disassembler -b instruction.bin -d data.bin
```

### Run the GUI

On Arch Linux, install the main dependencies:

```bash
sudo pacman -S --needed python python-pyside6 gcc
```

Then run:

```bash
cd SAC16_CPU-main/gui

python build_backend.py
python main.py
```

### Run the Verilog Simulation

After installing Icarus Verilog and GTKWave:

```bash
cd SAC16_Verilog_Arch

./scripts/run_program.sh \
    programs/working/1.bin \
    programs/working/1.data
```

To run the Yosys synthesis script:

```bash
./scripts/run_yosys_synth.sh
```

## Current Limitations

- The project was developed and tested in a Linux environment. Windows may require recompilation and additional configuration.
- The current assembler supports one `.data` section and one `.text` section per source file.
- `.word` is used for data definitions.
- Original label and variable names cannot be reconstructed by the disassembler.
- More advanced assembly and debugging features can be added in future versions.
- The RTL implementation has not yet been validated using Vivado.

## Documentation

For implementation details and explanations of individual components, see the documentation files in the `documents` directory and the RTL architecture explanation in `SAC16_Verilog_Arch`.

---

**SAC16 — A custom processor architecture, from assembly language to software simulation and Verilog RTL.**
