#include "flags.h"

void flags_update(Sac16State *cpu, const ALUResult *result)
{
    if (cpu == NULL || result == NULL)
        return;

    cpu->flags.zero = result->zero;
    cpu->flags.carry = result->carry;
}


void flags_reset(Sac16State *cpu)
{
    if (cpu == NULL)
        return;

    cpu->flags.zero = FALSE;
    cpu->flags.carry = FALSE;
}