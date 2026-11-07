#include "alu.h"

ALUResult alu_execute(word A, word B, Opcode operation)
{
    ALUResult alu;

    // Default values
    alu.result = 0;
    alu.zero = FALSE;
    alu.carry = FALSE;

    uint32_t temp = 0;

    switch (operation)
    {
        //================================
        // Arithmetic Operations
        //================================

        case ADD:
            temp = (uint32_t)A + (uint32_t)B;

            alu.result = (word)temp;
            alu.carry = (temp > 0xFFFF) ? TRUE : FALSE;
            break;

        case SUB:
            alu.result = (word)(A - B);

            // Carry = 1 means no borrow
            alu.carry = (A >= B) ? TRUE : FALSE;
            break;

        case MUL:
            temp = (uint32_t)A * (uint32_t)B;

            alu.result = (word)temp;
            alu.carry = (temp > 0xFFFF) ? TRUE : FALSE;
            break;

        //================================
        // Logical Operations
        //================================

        case AND:
            alu.result = A & B;
            break;

        case OR:
            alu.result = A | B;
            break;

        case NOT:
            alu.result = (word)(~A);
            break;

        default:
            return alu;
    }

    //================================
    // Zero Flag
    //================================

    alu.zero = (alu.result == 0) ? TRUE : FALSE;

    return alu;
}