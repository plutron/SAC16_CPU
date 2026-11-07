#pragma once

#include <stdint.h>
#include <stddef.h>

#include "constants.h"
#include "enums.h"

typedef uint16_t word;

typedef enum {
    CPU_RUN_HALTED,
    CPU_RUN_STEP_LIMIT,
    CPU_RUN_ERROR
} CpuRunStatus;

typedef struct
{
    word R[NUMBER_OF_REGISTERS];
    word ACC;
} Registers;

typedef struct
{
    Bool zero;
    Bool carry;
} Flags;

typedef struct
{
    word instructions[INSTRUCTION_MEMORY_SIZE];
    size_t counter;
} InstructionMemory;

typedef struct
{
    word data[DATA_MEMORY_SIZE];
    size_t counter;
} DataMemory;

typedef enum {
    CPU_ERROR_NONE,
    CPU_ERROR_INVALID_OPCODE,
    CPU_ERROR_INVALID_REGISTER,
    CPU_ERROR_INVALID_RESERVED_BITS,
    CPU_ERROR_INVALID_OPERAND,
    CPU_ERROR_STACK_UNDERFLOW,
    CPU_ERROR_STACK_OVERFLOW,
    CPU_ERROR_PC_OUT_OF_RANGE,
    CPU_ERROR_EXECUTION
} CpuError;


typedef struct
{
    Registers regs;
    InstructionMemory imem;
    DataMemory dmem;

    Flags flags;
    size_t stack_depth;
    CpuError error;
    word pc;
    word sp;

    Bool halted;
} Sac16State;


void cpu_init(Sac16State *cpu);
void cpu_reset(Sac16State *cpu);
Bool cpu_step(Sac16State *cpu);
Bool cpu_step(Sac16State *cpu);

CpuRunStatus cpu_run(Sac16State *cpu, size_t max_steps);
const char *cpu_error_string(CpuError error);