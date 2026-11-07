#include <stdio.h>
#include <string.h>

#include "decoder.h"


Bool instruction_decode(word instruction, DecodedInstruction *decoded)
{
    if (decoded == NULL)
    {
        fprintf(stderr, "Error: Invalid decoder argument.\n");
        return FALSE;
    }

    // Clear previous decoded data
    memset(decoded, 0, sizeof(DecodedInstruction));

    // Extract Opcode
    decoded->opcode = (Opcode)((instruction >> 12) & 0x0F);

    switch (decoded->opcode)
    {
        //-------------> R-Type
        case LD:
        case ST:
        case MOV:
        case ADD:
        case SUB:
        case MUL:
        case AND:
        case OR:

            decoded->rd = (RegisterName)((instruction >> 8) & 0x0F);
            decoded->rs = (RegisterName)((instruction >> 4) & 0x0F);

            decoded->reserved = instruction & 0x000F;

            break;


        //-------------> I-Type
        case LDI:

            decoded->rd = (RegisterName)((instruction >> 8) & 0x0F);
            decoded->imm = instruction & 0x00FF;

            break;


        //-------------> J-Type
        case JMP:
        case JZ:
        case JNZ:
        case CALL:

            decoded->target = instruction & 0x0FFF;

            break;


        //-------------> N-Type
        case NOT:
        case RET:
        case HALT:

            decoded->reserved = instruction & 0x0FFF;

            break;


        default:
            return FALSE;
    }

    return TRUE;
}