#include "control_unit.h"

Bool control_unit_execute(
    Sac16State *cpu,
    const DecodedInstruction *instruction)
{
    if (cpu == NULL || instruction == NULL)
        return FALSE;

    switch (instruction->opcode)
    {
        case JMP:
            cpu->pc = instruction->target;
            break;

        case JZ:
            if (cpu->regs.ACC == 0)
                cpu->pc = instruction->target;
            break;

        case JNZ:
            if (cpu->regs.ACC != 0)
                cpu->pc = instruction->target;
            break;

        case CALL:
            if (cpu->stack_depth >= DATA_MEMORY_SIZE)
            {
                cpu->error = CPU_ERROR_STACK_OVERFLOW;
                return FALSE;
            }

            cpu->dmem.data[cpu->sp] = cpu->pc;
            cpu->sp--;
            cpu->stack_depth++;

            cpu->pc = instruction->target;
            break;

        case RET:
            if (cpu->stack_depth == 0)
            {
                cpu->error = CPU_ERROR_STACK_UNDERFLOW;
                return FALSE;
            }

            cpu->sp++;
            cpu->pc = cpu->dmem.data[cpu->sp];
            cpu->stack_depth--;
            break;

        case HALT:
            cpu->halted = TRUE;
            break;

        default:
            cpu->error = CPU_ERROR_EXECUTION;
            return FALSE;
    }

    return TRUE;
}