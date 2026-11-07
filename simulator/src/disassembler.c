#include <stdio.h>
#include "disassembler.h"


static void print_register(FILE *output, int reg)
{
    if (reg == 8)
        fprintf(output, "ACC");
    else
        fprintf(output, "R%d", reg);
}

Bool disassemble_instruction(word instruction, FILE *output)
{
    if (output == NULL)
        return FALSE;

    DecodedInstruction decoded;

    if (!instruction_decode(instruction, &decoded))
        return FALSE;

    switch (decoded.opcode)
    {
        //-------------> R-Type

        case LD:
            fprintf(output, "LD ");
            print_register(output, decoded.rd);
            fprintf(output, ", ");
            print_register(output, decoded.rs);
            break;

        case ST:
            fprintf(output, "ST ");
            print_register(output, decoded.rd);
            fprintf(output, ", ");
            print_register(output, decoded.rs);
            break;

        case MOV:
            fprintf(output, "MOV ");
            print_register(output, decoded.rd);
            fprintf(output, ", ");
            print_register(output, decoded.rs);
            break;

        case ADD:
            fprintf(output, "ADD ");
            print_register(output, decoded.rd);
            break;

        case SUB:
            fprintf(output, "SUB ");
            print_register(output, decoded.rd);
            break;

        case MUL:
            fprintf(output, "MUL ");
            print_register(output, decoded.rd);
            break;

        case AND:
            fprintf(output, "AND ");
            print_register(output, decoded.rd);
            break;

        case OR:
            fprintf(output, "OR ");
            print_register(output, decoded.rd);
            break;


        //-------------> N-Type

        case NOT:
            fprintf(output, "NOT");
            break;

        case RET:
            fprintf(output, "RET");
            break;

        case HALT:
            fprintf(output, "HALT");
            break;


        //-------------> J-Type

        case JMP:
            fprintf(output, "JMP 0x%03X",
                    decoded.target);
            break;

        case JZ:
            fprintf(output, "JZ 0x%03X",
                    decoded.target);
            break;

        case JNZ:
            fprintf(output, "JNZ 0x%03X",
                    decoded.target);
            break;

        case CALL:
            fprintf(output, "CALL 0x%03X",
                    decoded.target);
            break;


        //-------------> I-Type

        case LDI:
            fprintf(output, "LDI ");
            print_register(output, decoded.rd);
            fprintf(output, ", %u", decoded.imm);
            break;


        default:
            return FALSE;
    }

    return TRUE;
}

Bool decoder(
    const char *input_filename,
    const char *output_filename
)
{
    if (input_filename == NULL || output_filename == NULL)
        return FALSE;

    FILE *input = fopen(input_filename, "rb");

    if (input == NULL)
        return FALSE;

    FILE *output = fopen(output_filename, "w");

    if (output == NULL)
    {
        fclose(input);
        return FALSE;
    }

    fprintf(output, ".text\n");

    size_t instruction_count = 0;
    Bool success = TRUE;

    while (1)
    {
        int high_byte = fgetc(input);

        if (high_byte == EOF)
        {
            if (ferror(input))
                success = FALSE;

            break;
        }

        int low_byte = fgetc(input);

        if (low_byte == EOF)
        {
            // فایل دارای تعداد بایت فرد است.
            success = FALSE;
            break;
        }

        // Big-Endian: ابتدا بایت پرارزش
        word instruction =
            ((word)high_byte << 8) | (word)low_byte;

        if (!disassemble_instruction(instruction, output))
        {
            // دستور نامعتبر است.
            success = FALSE;
            break;
        }

        fprintf(output, "\n");

        instruction_count++;
    }

    if (instruction_count == 0)
        success = FALSE;

    if (fclose(input) != 0)
        success = FALSE;

    if (fclose(output) != 0)
        success = FALSE;

    if (!success)
    {
        remove(output_filename);
        return FALSE;
    }

    printf(
        "[OK] Decoded %zu instructions into %s\n",
        instruction_count,
        output_filename
    );

    return TRUE;
}