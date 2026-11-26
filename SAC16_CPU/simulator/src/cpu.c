
#include <string.h>

#include "cpu.h"
#include "flags.h"
#include "decoder.h"
#include "datapath.h"
#include "control_unit.h"


// --------------------------------------------------
// CPU Reset
// --------------------------------------------------

void cpu_reset(Sac16State *cpu)
{
    if (cpu == NULL)
        return;

    memset(&cpu->regs, 0, sizeof(Registers));

    cpu->pc = 0x0000;
    cpu->sp = 0xFFFF;

    cpu->stack_depth = 0;
    cpu->error = CPU_ERROR_NONE;

    cpu->halted = FALSE;

    flags_reset(cpu);
}


// --------------------------------------------------
// Instruction Validation
// --------------------------------------------------

static CpuError validate_instruction(
    const DecodedInstruction *ins)
{
    if (ins == NULL)
        return CPU_ERROR_EXECUTION;

    switch (ins->opcode)
    {
        // R-Type instructions
        case LD:
        case ST:
        case MOV:

            if (ins->reserved != 0)
                return CPU_ERROR_INVALID_RESERVED_BITS;

            if (ins->rd > ACC || ins->rs > ACC)
                return CPU_ERROR_INVALID_REGISTER;

            return CPU_ERROR_NONE;


        // ALU instructions
        case ADD:
        case SUB:
        case MUL:
        case AND:
        case OR:

            if (ins->reserved != 0)
                return CPU_ERROR_INVALID_RESERVED_BITS;

            if (ins->rd > ACC)
                return CPU_ERROR_INVALID_REGISTER;

            if (ins->rs != 0)
                return CPU_ERROR_INVALID_OPERAND;

            return CPU_ERROR_NONE;


        // N-Type instructions
        case NOT:
        case RET:
        case HALT:

            if (ins->reserved != 0)
                return CPU_ERROR_INVALID_RESERVED_BITS;

            return CPU_ERROR_NONE;


        // I-Type instruction
        case LDI:

            if (ins->rd > ACC)
                return CPU_ERROR_INVALID_REGISTER;

            return CPU_ERROR_NONE;


        // J-Type instructions
        case JMP:
        case JZ:
        case JNZ:
        case CALL:

            return CPU_ERROR_NONE;


        default:
            return CPU_ERROR_INVALID_OPCODE;
    }
}


// --------------------------------------------------
// CPU Error Message
// --------------------------------------------------

const char *cpu_error_string(CpuError error)
{
    switch (error)
    {
        case CPU_ERROR_NONE:
            return "No error";

        case CPU_ERROR_INVALID_OPCODE:
            return "Invalid opcode";

        case CPU_ERROR_INVALID_REGISTER:
            return "Invalid register";

        case CPU_ERROR_INVALID_RESERVED_BITS:
            return "Reserved bits must be zero";

        case CPU_ERROR_INVALID_OPERAND:
            return "Invalid operand";

        case CPU_ERROR_STACK_UNDERFLOW:
            return "Stack underflow";

        case CPU_ERROR_STACK_OVERFLOW:
            return "Stack overflow";

        case CPU_ERROR_PC_OUT_OF_RANGE:
            return "Program counter out of range";

        case CPU_ERROR_EXECUTION:
            return "Instruction execution failed";

        default:
            return "Unknown CPU error";
    }
}


// --------------------------------------------------
// CPU Step
// --------------------------------------------------

Bool cpu_step(Sac16State *cpu)
{
    if (cpu == NULL)
        return FALSE;

    // CPU has already halted
    if (cpu->halted)
        return TRUE;

    // Clear previous error
    cpu->error = CPU_ERROR_NONE;


    // 1. Check PC
    if (cpu->pc >= INSTRUCTION_MEMORY_SIZE)
    {
        cpu->error = CPU_ERROR_PC_OUT_OF_RANGE;
        return FALSE;
    }


    // 2. Fetch
    word instruction_word =
        cpu->imem.instructions[cpu->pc];


    // 3. Increment PC
    cpu->pc++;


    // 4. Decode
    DecodedInstruction instruction;

    if (!instruction_decode(
            instruction_word,
            &instruction))
    {
        cpu->error = CPU_ERROR_INVALID_OPCODE;
        return FALSE;
    }


    // 5. Validate
    CpuError validation =
        validate_instruction(&instruction);

    if (validation != CPU_ERROR_NONE)
    {
        cpu->error = validation;
        return FALSE;
    }


    // 6. Execute
    Bool result;

    switch (instruction.opcode)
    {
        case JMP:
        case JZ:
        case JNZ:
        case CALL:
        case RET:
        case HALT:

            result = control_unit_execute(
                cpu,
                &instruction
            );

            break;


        default:

            result = datapath_execute(
                cpu,
                &instruction
            );

            break;
    }


    // 7. Handle execution error
    if (!result)
    {
        if (cpu->error == CPU_ERROR_NONE)
            cpu->error = CPU_ERROR_EXECUTION;

        return FALSE;
    }

    return TRUE;
}


// --------------------------------------------------
// CPU Run
// --------------------------------------------------

CpuRunStatus cpu_run(
    Sac16State *cpu,
    size_t max_steps)
{
    if (cpu == NULL)
        return CPU_RUN_ERROR;

    if (cpu->halted)
        return CPU_RUN_HALTED;


    for (size_t i = 0; i < max_steps; i++)
    {
        if (!cpu_step(cpu))
            return CPU_RUN_ERROR;

        if (cpu->halted)
            return CPU_RUN_HALTED;
    }

    return CPU_RUN_STEP_LIMIT;
}


// --------------------------------------------------
// CPU Initialization
// --------------------------------------------------

void cpu_init(Sac16State *cpu)
{
    if (cpu == NULL)
        return;

    memset(cpu, 0, sizeof(Sac16State));

    cpu_reset(cpu);
}
