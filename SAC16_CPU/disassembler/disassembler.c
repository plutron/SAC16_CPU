
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_PATH_LENGTH 1024

typedef uint16_t word;

typedef enum
{
    LD = 0x0,
    ST = 0x1,
    MOV = 0x2,
    ADD = 0x3,
    SUB = 0x4,
    MUL = 0x5,
    AND = 0x6,
    OR  = 0x7,
    NOT = 0x8,
    JMP = 0x9,
    JZ  = 0xA,
    JNZ = 0xB,
    CALL = 0xC,
    RET = 0xD,
    LDI = 0xE,
    HALT = 0xF
} Opcode;


// Convert register number to assembly name
static void print_register(FILE *output, unsigned int reg)
{
    if (reg == 8)
        fprintf(output, "ACC");
    else
        fprintf(output, "R%u", reg);
}


// Decode one instruction
static int disassemble_instruction(word instruction, FILE *output)
{
    unsigned int opcode = (instruction >> 12) & 0xF;
    unsigned int a = (instruction >> 8) & 0xF;
    unsigned int b = (instruction >> 4) & 0xF;
    unsigned int reserved = instruction & 0xF;

    switch (opcode)
    {
        case LD:
            if (a > 8 || b > 8 || reserved != 0)
                return 0;

            fprintf(output, "    LD ");
            print_register(output, a);
            fprintf(output, ", ");
            print_register(output, b);
            fprintf(output, "\n");
            break;

        case ST:
            if (a > 8 || b > 8 || reserved != 0)
                return 0;

            fprintf(output, "    ST ");
            print_register(output, a);
            fprintf(output, ", ");
            print_register(output, b);
            fprintf(output, "\n");
            break;

        case MOV:
            if (a > 8 || b > 8 || reserved != 0)
                return 0;

            fprintf(output, "    MOV ");
            print_register(output, a);
            fprintf(output, ", ");
            print_register(output, b);
            fprintf(output, "\n");
            break;

        case ADD:
        case SUB:
        case MUL:
        case AND:
        case OR:
            if (a > 8 || b != 0 || reserved != 0)
                return 0;

            fprintf(output, "    ");

            switch (opcode)
            {
                case ADD: fprintf(output, "ADD "); break;
                case SUB: fprintf(output, "SUB "); break;
                case MUL: fprintf(output, "MUL "); break;
                case AND: fprintf(output, "AND "); break;
                case OR:  fprintf(output, "OR ");  break;
            }

            print_register(output, a);
            fprintf(output, "\n");
            break;

        case NOT:
            if ((instruction & 0x0FFF) != 0)
                return 0;

            fprintf(output, "    NOT\n");
            break;

        case JMP:
        case JZ:
        case JNZ:
        case CALL:
        {
            unsigned int target = instruction & 0x0FFF;

            fprintf(output, "    ");

            switch (opcode)
            {
                case JMP:  fprintf(output, "JMP ");  break;
                case JZ:   fprintf(output, "JZ ");   break;
                case JNZ:  fprintf(output, "JNZ ");  break;
                case CALL: fprintf(output, "CALL "); break;
            }

            fprintf(output, "0x%03X\n", target);
            break;
        }

        case RET:
            if ((instruction & 0x0FFF) != 0)
                return 0;

            fprintf(output, "    RET\n");
            break;

        case LDI:
            if (a > 8)
                return 0;

            fprintf(output, "    LDI ");
            print_register(output, a);
            fprintf(output, ", %u\n", instruction & 0xFF);
            break;

        case HALT:
            if ((instruction & 0x0FFF) != 0)
                return 0;

            fprintf(output, "    HALT\n");
            break;

        default:
            return 0;
    }

    return !ferror(output);
}


// Decode instruction memory
static int decode_instruction_file(
    const char *filename,
    FILE *output)
{
    FILE *input = fopen(filename, "rb");

    if (input == NULL)
    {
        fprintf(stderr, "Error: Cannot open %s\n", filename);
        return 0;
    }

    if (fseek(input, 0, SEEK_END) != 0)
    {
        fclose(input);
        return 0;
    }

    long file_size = ftell(input);

    if (file_size < 0 || file_size % 2 != 0)
    {
        fprintf(stderr, "Error: Invalid instruction file size.\n");
        fclose(input);
        return 0;
    }

    rewind(input);

    if (file_size == 0)
    {
        fprintf(stderr, "Error: Instruction file is empty.\n");
        fclose(input);
        return 0;
    }

    fprintf(output, ".text\n");

    size_t count = 0;

    while (count < (size_t)file_size / 2)
    {
        int high = fgetc(input);
        int low = fgetc(input);

        if (high == EOF || low == EOF)
        {
            fclose(input);
            return 0;
        }

        word instruction =
            ((word)(uint8_t)high << 8) |
            (word)(uint8_t)low;

        if (!disassemble_instruction(instruction, output))
        {
            fprintf(stderr,
                    "Error: Invalid instruction at address 0x%03zX\n",
                    count);

            fclose(input);
            return 0;
        }

        count++;
    }

    fclose(input);

    printf("[OK] Decoded %zu instructions.\n", count);

    return 1;
}


// Decode data memory
static int decode_data_file(
    const char *filename,
    FILE *output)
{
    FILE *input = fopen(filename, "rb");

    if (input == NULL)
    {
        fprintf(stderr, "Error: Cannot open %s\n", filename);
        return 0;
    }

    if (fseek(input, 0, SEEK_END) != 0)
    {
        fclose(input);
        return 0;
    }

    long file_size = ftell(input);

    if (file_size < 0 || file_size % 2 != 0)
    {
        fprintf(stderr, "Error: Invalid data file size.\n");
        fclose(input);
        return 0;
    }

    rewind(input);

    fprintf(output, "\n.data\n");

    size_t count = 0;

    while (count < (size_t)file_size / 2)
    {
        int high = fgetc(input);
        int low = fgetc(input);

        if (high == EOF || low == EOF)
        {
            fclose(input);
            return 0;
        }

        word value =
            ((word)(uint8_t)high << 8) |
            (word)(uint8_t)low;

        fprintf(output, "    .word %u\n", value);

        count++;
    }

    fclose(input);

    printf("[OK] Decoded %zu data words.\n", count);

    return !ferror(output);
}


// Create output filename from binary filename
static int create_output_filename(
    const char *input,
    char *output,
    size_t output_size)
{
    size_t length = strlen(input);

    if (length + 5 > output_size)
        return 0;

    strcpy(output, input);

    char *dot = strrchr(output, '.');

    if (dot != NULL)
        strcpy(dot, ".asm");
    else
        strcat(output, ".asm");

    return 1;
}


static void print_usage(const char *program)
{
    printf("Usage:\n");
    printf("  %s -b instruction.bin\n", program);
    printf("  %s -b instruction.bin -d data.bin\n", program);
}


int main(int argc, char *argv[])
{
    char *instruction_file = NULL;
    char *data_file = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-b") == 0 && i + 1 < argc)
        {
            instruction_file = argv[++i];
        }
        else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc)
        {
            data_file = argv[++i];
        }
        else
        {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (instruction_file == NULL)
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    char output_file[MAX_PATH_LENGTH];

    if (!create_output_filename(
            instruction_file,
            output_file,
            sizeof(output_file)))
    {
        fprintf(stderr, "Error: Output filename is too long.\n");
        return EXIT_FAILURE;
    }

    FILE *output = fopen(output_file, "w");

    if (output == NULL)
    {
        fprintf(stderr, "Error: Cannot create %s\n", output_file);
        return EXIT_FAILURE;
    }

    printf("[1] Decoding Instruction Memory...\n");

    if (!decode_instruction_file(instruction_file, output))
    {
        fclose(output);
        remove(output_file);
        return EXIT_FAILURE;
    }

    if (data_file != NULL)
    {
        printf("\n[2] Decoding Data Memory...\n");

        if (!decode_data_file(data_file, output))
        {
            fclose(output);
            remove(output_file);
            return EXIT_FAILURE;
        }
    }

    if (fclose(output) != 0)
    {
        remove(output_file);
        fprintf(stderr, "Error: Failed to write output file.\n");
        return EXIT_FAILURE;
    }

    printf("\n[OK] Disassembly completed.\n");
    printf("Output: %s\n", output_file);

    return EXIT_SUCCESS;
}
