#pragma once
#include <stdint.h>
#include "enums.h"
#include "constants.h"

typedef uint16_t word;   // a 16-bit machine word

typedef struct
{
    Opcode   opcode;     // instruction opcode
    RegisterName rd;         // destination register
    RegisterName rs;         // source register
    uint8_t  imm8;       // 8-bit immediate
    uint16_t target;     // jump/branch target address
} Instruction;

typedef struct {
    Instruction instructions[INSTRUCTION_MEMORY_SIZE];
    size_t         counter;
} InstructionMemory;

typedef struct {
    word data[DATA_MEMORY_SIZE];
    size_t counter;
} DataMemory;

typedef struct{
    char label[MAX_SIZE_OF_LABEL_NAME];
    word address;
    TypeOfMemory type;
}SymbolTableElement;

typedef struct {
    InstructionMemory imem;
    DataMemory        dmem;
    SymbolTableElement sytable[SYMBOL_TABLE_SIZE];
    size_t sytable_counter;
} Sac16State;