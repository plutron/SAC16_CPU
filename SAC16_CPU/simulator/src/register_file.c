#include "register_file.h"


Bool register_file_read(
    Sac16State *cpu,
    RegisterName reg,
    word *value
)
{
    if (cpu == NULL || value == NULL)
        return FALSE;

    // General-purpose registers: R0-R7
    if (reg >= R0 && reg <= R7)
    {
        *value = cpu->regs.R[reg];
        return TRUE;
    }

    // Accumulator
    if (reg == ACC)
    {
        *value = cpu->regs.ACC;
        return TRUE;
    }

    return FALSE;
}


Bool register_file_write(
    Sac16State *cpu,
    RegisterName reg,
    word value
)
{
    if (cpu == NULL)
        return FALSE;

    // General-purpose registers: R0-R7
    if (reg >= R0 && reg <= R7)
    {
        cpu->regs.R[reg] = value;
        return TRUE;
    }

    // Accumulator
    if (reg == ACC)
    {
        cpu->regs.ACC = value;
        return TRUE;
    }

    return FALSE;
}