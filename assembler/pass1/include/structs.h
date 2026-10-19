#pragma once
#include <stdint.h>
#include "enums.h"
#include "constants.h"

typedef uint16_t word;   // a 16-bit machine word

typedef struct
{
    Opcode   opcode;     // instruction opcode
    RegisterName rs;         // source register
    RegisterName rd;         // destination register
    uint8_t  imm8;       // 8-bit immediate
    uint16_t target;     // jump/branch target address
} Instruction;

typedef struct {
    Instruction instructions[INSTRUCTION_MEMORY_SIZE];
    int         counter;
} InstructionMemory;

typedef struct {
    word data[DATA_MEMORY_SIZE];
} DataMemory;

typedef struct {
    InstructionMemory imem;
    DataMemory        dmem;
    word          register_file[NUMBER_OF_REGISTERS];   // R0-R7 (index = register code)
    word          acc;
    word          pc;
    word          sp;
    Bool          halted;
} Sac16State;