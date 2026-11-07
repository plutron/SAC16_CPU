#pragma once

#include "cpu.h"

typedef struct
{
    word result;
    Bool zero;
    Bool carry;
} ALUResult;

ALUResult alu_execute(word A, word B, Opcode operation);