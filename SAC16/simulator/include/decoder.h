#pragma once

#include <stdint.h>
#include "cpu.h"

typedef struct
{
    Opcode opcode;

    // Register operands
    RegisterName rd;
    RegisterName rs;

    // Immediate operand
    uint8_t imm;

    // Jump address
    uint16_t target;

    // Reserved bits
    uint16_t reserved;

} DecodedInstruction;


//-------------> Instruction Decoder
Bool instruction_decode(word instruction, DecodedInstruction *decoded);