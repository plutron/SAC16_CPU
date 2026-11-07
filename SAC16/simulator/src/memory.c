#include <stdio.h>
#include "memory.h"

Bool memory_read_instruction(Sac16State *cpu, word address, word *result)
{
    if (cpu == NULL || result == NULL) {
        fprintf(stderr, "Error: Invalid memory access arguments.\n");
        return FALSE;
    }

    if (address >= INSTRUCTION_MEMORY_SIZE) {
        fprintf(stderr,
                "Error: Instruction memory address out of range: 0x%04X\n",
                address);
        return FALSE;
    }

    *result = cpu->imem.instructions[address];

    return TRUE;
}

Bool memory_read_data(Sac16State *cpu, word address, word *result)
{
    if (cpu == NULL || result == NULL) {
        fprintf(stderr, "Error: Invalid memory access arguments.\n");
        return FALSE;
    }

    *result = cpu->dmem.data[address];

    return TRUE;
}


Bool memory_write_data(Sac16State *cpu, word address, word value)
{
    if (cpu == NULL) {
        fprintf(stderr, "Error: Invalid memory access arguments.\n");
        return FALSE;
    }

    cpu->dmem.data[address] = value;

    return TRUE;
}

Bool memory_load_instruction(Sac16State *cpu, const char *filename)
{
    if (cpu == NULL || filename == NULL)
    {
        fprintf(stderr, "Error: Invalid arguments for instruction loading.\n");
        return FALSE;
    }

    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        fprintf(stderr, "Error: Cannot open instruction file: %s\n", filename);
        return FALSE;
    }

    // Find file size
    if (fseek(file, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "Error: Cannot seek instruction file.\n");
        fclose(file);
        return FALSE;
    }

    long file_size = ftell(file);

    if (file_size < 0)
    {
        fprintf(stderr, "Error: Cannot determine instruction file size.\n");
        fclose(file);
        return FALSE;
    }

    // Check empty file
    if (file_size == 0)
    {
        fprintf(stderr, "Error: Instruction file is empty.\n");
        fclose(file);
        return FALSE;
    }

    // Each instruction occupies 2 bytes
    if (file_size % 2 != 0)
    {
        fprintf(stderr, "Error: Invalid instruction file size (odd number of bytes).\n");
        fclose(file);
        return FALSE;
    }

    // Check instruction memory capacity
    size_t instruction_count = (size_t)file_size / 2;

    if (instruction_count > INSTRUCTION_MEMORY_SIZE)
    {
        fprintf(stderr, "Error: Instruction memory overflow.\n");
        fclose(file);
        return FALSE;
    }

    // Return to beginning of file
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        fprintf(stderr, "Error: Cannot rewind instruction file.\n");
        fclose(file);
        return FALSE;
    }

    // Temporary buffer
    unsigned char buffer[INSTRUCTION_MEMORY_SIZE * 2];

    size_t bytes_read = fread(
        buffer,
        1,
        (size_t)file_size,
        file
    );

    if (bytes_read != (size_t)file_size || ferror(file))
    {
        fprintf(stderr, "Error: Failed to read instruction file.\n");
        fclose(file);
        return FALSE;
    }

    if (fclose(file) != 0)
    {
        fprintf(stderr, "Error: Failed to close instruction file.\n");
        return FALSE;
    }

    // Load instructions into IMEM
    for (size_t i = 0; i < instruction_count; i++)
    {
        cpu->imem.instructions[i] =
            ((word)buffer[i * 2] << 8) |
            (word)buffer[i * 2 + 1];
    }

    // Update instruction counter
    cpu->imem.counter = instruction_count;

    return TRUE;
}

Bool memory_load_data(Sac16State *cpu, const char *filename)
{
    if (cpu == NULL || filename == NULL) {
        fprintf(stderr, "Error: Invalid arguments for data loading.\n");
        return FALSE;
    }

    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open data file: %s\n", filename);
        return FALSE;
    }

    unsigned char buffer[2];
    size_t count = 0;

    while (1) {

        size_t bytes_read = fread(buffer, 1, 2, file);

        if (bytes_read == 0)
            break;

        if (bytes_read != 2) {
            fprintf(stderr,
                    "Error: Invalid data file size (odd number of bytes).\n");
            fclose(file);
            return FALSE;
        }

        if (count >= DATA_MEMORY_SIZE) {
            fprintf(stderr, "Error: Data memory overflow.\n");
            fclose(file);
            return FALSE;
        }

        cpu->dmem.data[count] =
            ((word)buffer[0] << 8) | buffer[1];

        count++;
    }

    if (ferror(file)) {
        fprintf(stderr, "Error: Failed to read data file.\n");
        fclose(file);
        return FALSE;
    }

    cpu->dmem.counter = count;

    fclose(file);

    return TRUE;
}