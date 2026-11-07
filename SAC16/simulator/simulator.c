
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "cpu.h"
#include "memory.h"
#include "disassembler.h"

#define MAX_PATH_LENGTH 1024
#define ASSEMBLER_PATH "../../assembler/sac16_asm"

#define BIN_FILE "a.bin"
#define DATA_FILE "a.data"

#define MAX_SIMULATION_STEPS 1000000
#define MAX_BREAKPOINTS 64
#define MAX_WATCHPOINTS 16

/* =========================
   Simulation Mode
   ========================= */

typedef enum
{
    MODE_NORMAL,
    MODE_STEP,
    MODE_TRACE
} SimulationMode;

/* =========================
   Breakpoints
   ========================= */

typedef struct
{
    word addresses[MAX_BREAKPOINTS];
    size_t count;

} BreakpointSet;

/* =========================
   Execution Statistics
   ========================= */

typedef struct
{
    size_t total_instructions;
    size_t opcode_count[16];

    size_t memory_operations;
    size_t stack_operations;

    size_t conditional_branches;
    size_t taken_branches;
    size_t not_taken_branches;
    size_t control_transfers;

    size_t total_cycles;

} ExecutionStats;

/* =========================
   Watchpoints
   ========================= */

typedef enum
{
    WATCH_REGISTER,
    WATCH_MEMORY
} WatchType;

typedef struct
{
    Bool enabled;
    Bool triggered;

    WatchType type;
    unsigned int index;

    word old_value;
    word new_value;

} Watchpoint;

typedef struct
{
    Watchpoint items[MAX_WATCHPOINTS];
    size_t count;

} WatchpointSet;

/* =========================
   Opcode Names
   ========================= */

static const char *opcode_names[16] =
{
    "LD", "ST", "MOV", "ADD",
    "SUB", "MUL", "AND", "OR",
    "NOT", "JMP", "JZ", "JNZ",
    "CALL", "RET", "LDI", "HALT"
};

/* =========================
   Function Prototypes
   ========================= */

static void print_usage(const char *program_name);

static void print_cpu_state(const Sac16State *cpu);
static void print_instruction_memory(const Sac16State *cpu);
static void print_data_memory(const Sac16State *cpu);

static void print_changed_data_memory(
    const word *previous,
    const Sac16State *cpu
);

static void stats_init(ExecutionStats *stats);
static void stats_print(const ExecutionStats *stats);

static size_t get_instruction_cycles(Opcode opcode);

static Bool execute_one_instruction(
    Sac16State *cpu,
    ExecutionStats *stats,
    WatchpointSet *watchpoints
);

static Bool parse_breakpoints(
    const char *input,
    BreakpointSet *breakpoints
);

static Bool is_breakpoint(
    const BreakpointSet *breakpoints,
    word address
);

static void handle_breakpoint(
    Sac16State *cpu,
    const BreakpointSet *breakpoints,
    Bool wait_for_enter
);

static Bool watchpoint_add(
    WatchpointSet *watchpoints,
    WatchType type,
    unsigned int index
);

static Bool parse_watch_register(
    const char *input,
    unsigned int *reg
);

static Bool parse_watch_address(
    const char *input,
    unsigned int *address
);

static word watchpoint_read_value(
    const Sac16State *cpu,
    const Watchpoint *wp
);

static Bool watchpoint_has_triggered(
    const WatchpointSet *watchpoints
);

static void watchpoint_report(const Watchpoint *wp);

static void print_watchpoints(
    const WatchpointSet *watchpoints
);

static Bool run_normal_mode(
    Sac16State *cpu,
    ExecutionStats *stats,
    const BreakpointSet *breakpoints,
    WatchpointSet *watchpoints
);

static Bool run_step_mode(
    Sac16State *cpu,
    ExecutionStats *stats,
    const BreakpointSet *breakpoints,
    WatchpointSet *watchpoints
);

static Bool run_trace_mode(
    Sac16State *cpu,
    ExecutionStats *stats,
    const BreakpointSet *breakpoints,
    WatchpointSet *watchpoints
);

/* =========================
   Usage
   ========================= */

static void print_usage(const char *program_name)
{
    printf("Usage:\n\n");

    printf("  %s <assembly_file>\n", program_name);

    printf("  %s --step <assembly_file>\n", program_name);

    printf("  %s --trace <assembly_file>\n", program_name);

    printf("\nBreakpoints:\n");

    printf("  %s --break 5 <assembly_file>\n", program_name);

    printf("  %s --break 5,8,12 <assembly_file>\n", program_name);

    printf("\nWatchpoints:\n");

    printf("  %s --watch R2 <assembly_file>\n", program_name);

    printf("  %s --watch ACC <assembly_file>\n", program_name);

    printf("  %s --watch-mem 0x0000 <assembly_file>\n", program_name);

    printf("\nCombined:\n");

    printf("  %s --trace --break 5 --watch ACC a.asm\n",
           program_name);
}

/* =========================
   CPU State
   ========================= */

static void print_cpu_state(const Sac16State *cpu)
{
    if (cpu == NULL)
        return;

    printf("\n");
    printf("================================\n");
    printf("             CPU STATE\n");
    printf("================================\n");

    printf("PC:      0x%04X (%u)\n", cpu->pc, cpu->pc);
    printf("SP:      0x%04X (%u)\n", cpu->sp, cpu->sp);

    printf("ACC:     0x%04X (%u)\n",
           cpu->regs.ACC, cpu->regs.ACC);

    printf("\nRegisters:\n");

    for (size_t i = 0; i < NUMBER_OF_REGISTERS; i++)
    {
        printf("R%-2zu:     0x%04X (%u)\n",
               i,
               cpu->regs.R[i],
               cpu->regs.R[i]);
    }

    printf("\nFlags:\n");

    printf("Zero:    %d\n", cpu->flags.zero);
    printf("Carry:   %d\n", cpu->flags.carry);

    printf("\nStack depth: %zu\n", cpu->stack_depth);

    printf("\nStatus:\n");

    printf("Halted:  %s\n", cpu->halted ? "Yes" : "No");

    printf("Error:   %s\n", cpu_error_string(cpu->error));

    printf("================================\n");
}

/* =========================
   Instruction Memory
   ========================= */

static void print_instruction_memory(const Sac16State *cpu)
{
    if (cpu == NULL)
        return;

    printf("\n");
    printf("================================\n");
    printf("       INSTRUCTION MEMORY\n");
    printf("================================\n");

    printf("%-8s %-10s %-20s\n",
           "Address",
           "Machine",
           "Instruction");

    printf("--------------------------------\n");

    for (size_t i = 0; i < cpu->imem.counter; i++)
    {
        printf("0x%04zX   0x%04X     ",
               i,
               cpu->imem.instructions[i]);

        disassemble_instruction(
            cpu->imem.instructions[i],
            stdout
        );

        printf("\n");
    }

    printf("================================\n");
}

/* =========================
   Data Memory
   ========================= */

static void print_data_memory(const Sac16State *cpu)
{
    if (cpu == NULL)
        return;

    printf("\n");
    printf("================================\n");
    printf("          DATA MEMORY\n");
    printf("================================\n");

    printf("%-10s %-10s %-10s\n",
           "Address",
           "Hex",
           "Decimal");

    printf("--------------------------------\n");

    for (size_t i = 0; i < cpu->dmem.counter; i++)
    {
        printf("0x%04zX    0x%04X     %u\n",
               i,
               cpu->dmem.data[i],
               cpu->dmem.data[i]);
    }

    printf("================================\n");
}

/* =========================
   Changed Data Memory
   ========================= */

static void print_changed_data_memory(
    const word *previous,
    const Sac16State *cpu
)
{
    if (previous == NULL || cpu == NULL)
        return;

    Bool changed = FALSE;

    for (size_t i = 0; i < DATA_MEMORY_SIZE; i++)
    {
        if (previous[i] != cpu->dmem.data[i])
        {
            if (!changed)
            {
                printf("\nChanged Data Memory:\n");
                changed = TRUE;
            }

            printf("[%04zX] 0x%04X (%u) -> 0x%04X (%u)\n",
                   i,
                   previous[i],
                   previous[i],
                   cpu->dmem.data[i],
                   cpu->dmem.data[i]);
        }
    }

    if (!changed)
        printf("\nNo Data Memory changes.\n");
}

/* =========================
   Execution Statistics
   ========================= */

static void stats_init(ExecutionStats *stats)
{
    if (stats == NULL)
        return;

    memset(stats, 0, sizeof(*stats));
}

static void stats_print(const ExecutionStats *stats)
{
    if (stats == NULL)
        return;

    printf("\n");
    printf("================================\n");
    printf("       EXECUTION STATISTICS\n");
    printf("================================\n");

    printf("Total instructions: %zu\n",
           stats->total_instructions);

    printf("\nOpcode frequency:\n");
    printf("--------------------------------\n");

    for (size_t i = 0; i < 16; i++)
    {
        if (stats->opcode_count[i] > 0)
        {
            printf("%-8s : %zu\n",
                   opcode_names[i],
                   stats->opcode_count[i]);
        }
    }

    printf("--------------------------------\n");

    printf("\nMemory operations (LD/ST): %zu\n",
           stats->memory_operations);

    printf("Stack operations (CALL/RET): %zu\n",
           stats->stack_operations);

    printf("\nConditional branches:\n");

    printf("Total:       %zu\n",
           stats->conditional_branches);

    printf("Taken:       %zu\n",
           stats->taken_branches);

    printf("Not taken:   %zu\n",
           stats->not_taken_branches);

    printf("\nControl transfers: %zu\n",
           stats->control_transfers);

    double cpi = 0.0;

    if (stats->total_instructions > 0)
    {
        cpi = (double)stats->total_cycles /
              (double)stats->total_instructions;
    }

    printf("\nCycle Statistics\n");
    printf("--------------------------------\n");

    printf("Total Cycles : %zu\n", stats->total_cycles);
    printf("Average CPI  : %.2f\n", cpi);

    printf("================================\n");
}

/* =========================
   Cycle Counter
   ========================= */

static size_t get_instruction_cycles(Opcode opcode)
{
    switch (opcode)
    {
        case LD:
        case ST:
            return 2;

        case MUL:
            return 3;

        case CALL:
        case RET:
            return 2;

        case MOV:
        case ADD:
        case SUB:
        case AND:
        case OR:
        case NOT:
        case JMP:
        case JZ:
        case JNZ:
        case LDI:
        case HALT:
            return 1;

        default:
            return 0;
    }
}

/* =========================
   Watchpoint
   ========================= */

static Bool watchpoint_add(
    WatchpointSet *watchpoints,
    WatchType type,
    unsigned int index
)
{
    if (watchpoints == NULL)
        return FALSE;

    /* Avoid duplicate Watchpoints */

    for (size_t i = 0; i < watchpoints->count; i++)
    {
        Watchpoint *wp = &watchpoints->items[i];

        if (wp->type == type && wp->index == index)
            return TRUE;
    }

    if (watchpoints->count >= MAX_WATCHPOINTS)
        return FALSE;

    Watchpoint *wp =
        &watchpoints->items[watchpoints->count++];

    wp->enabled = TRUE;
    wp->triggered = FALSE;

    wp->type = type;
    wp->index = index;

    wp->old_value = 0;
    wp->new_value = 0;

    return TRUE;
}


static Bool parse_watch_register(
    const char *input,
    unsigned int *reg
)
{
    if (input == NULL || reg == NULL)
        return FALSE;

    if (strcmp(input, "ACC") == 0)
    {
        *reg = 8;
        return TRUE;
    }

    if (input[0] != 'R' || input[1] == '\0')
        return FALSE;

    if (input[2] != '\0')
        return FALSE;

    if (input[1] < '0' || input[1] > '7')
        return FALSE;

    *reg = (unsigned int)(input[1] - '0');

    return TRUE;
}


static Bool parse_watch_address(
    const char *input,
    unsigned int *address
)
{
    if (input == NULL || address == NULL)
        return FALSE;

    if (input[0] == '\0' || input[0] == '-')
        return FALSE;

    char *end = NULL;

    unsigned long value = strtoul(input, &end, 0);

    if (end == input || *end != '\0')
        return FALSE;

    if (value >= DATA_MEMORY_SIZE)
        return FALSE;

    *address = (unsigned int)value;

    return TRUE;
}


static word watchpoint_read_value(
    const Sac16State *cpu,
    const Watchpoint *wp
)
{
    if (wp->type == WATCH_MEMORY)
        return cpu->dmem.data[wp->index];

    if (wp->index == 8)
        return cpu->regs.ACC;

    return cpu->regs.R[wp->index];
}


static Bool watchpoint_has_triggered(
    const WatchpointSet *watchpoints
)
{
    if (watchpoints == NULL)
        return FALSE;

    for (size_t i = 0; i < watchpoints->count; i++)
    {
        if (watchpoints->items[i].triggered)
            return TRUE;
    }

    return FALSE;
}


static void watchpoint_report(const Watchpoint *wp)
{
    if (wp == NULL)
        return;

    printf("\n");
    printf("%s", YELLOW);

    printf("================================\n");
    printf("        WATCHPOINT HIT\n");
    printf("================================\n");

    if (wp->type == WATCH_REGISTER)
    {
        if (wp->index == 8)
            printf("Register: ACC\n");
        else
            printf("Register: R%u\n", wp->index);
    }
    else
    {
        printf("Memory Address: 0x%04X\n", wp->index);
    }

    printf("Old Value: 0x%04X (%u)\n",
           wp->old_value,
           wp->old_value);

    printf("New Value: 0x%04X (%u)\n",
           wp->new_value,
           wp->new_value);

    printf("================================\n");

    printf("%s", RESET);
}


static void print_watchpoints(
    const WatchpointSet *watchpoints
)
{
    if (watchpoints == NULL || watchpoints->count == 0)
        return;

    printf("\nWatchpoints:\n");

    for (size_t i = 0; i < watchpoints->count; i++)
    {
        const Watchpoint *wp = &watchpoints->items[i];

        if (wp->type == WATCH_REGISTER)
        {
            if (wp->index == 8)
                printf("  ACC\n");
            else
                printf("  R%u\n", wp->index);
        }
        else
        {
            printf("  Memory[0x%04X]\n", wp->index);
        }
    }
}

/* =========================
   Execute One Instruction
   ========================= */

static Bool execute_one_instruction(
    Sac16State *cpu,
    ExecutionStats *stats,
    WatchpointSet *watchpoints
)
{
    if (cpu == NULL || stats == NULL)
        return FALSE;

    if (cpu->pc >= cpu->imem.counter)
    {
        cpu->error = CPU_ERROR_PC_OUT_OF_RANGE;
        return FALSE;
    }

    word instruction = cpu->imem.instructions[cpu->pc];

    unsigned int opcode =
        (instruction >> 12) & 0x0F;

    Bool previous_zero = cpu->flags.zero;

    /*
     * Save old values before execution.
     */

    if (watchpoints != NULL)
    {
        for (size_t i = 0; i < watchpoints->count; i++)
        {
            Watchpoint *wp = &watchpoints->items[i];

            if (wp->enabled && !wp->triggered)
            {
                wp->old_value =
                    watchpoint_read_value(cpu, wp);
            }
        }
    }

    /*
     * Execute instruction.
     */

    if (!cpu_step(cpu))
        return FALSE;

    /*
     * Update statistics.
     */

    stats->total_instructions++;
    stats->opcode_count[opcode]++;

    switch (opcode)
    {
        case LD:
        case ST:
            stats->memory_operations++;
            break;

        case CALL:
        case RET:
            stats->stack_operations++;
            stats->control_transfers++;
            break;

        case JMP:
            stats->control_transfers++;
            break;

        case JZ:
            stats->conditional_branches++;

            if (previous_zero)
            {
                stats->taken_branches++;
                stats->control_transfers++;
            }
            else
            {
                stats->not_taken_branches++;
            }

            break;

        case JNZ:
            stats->conditional_branches++;

            if (!previous_zero)
            {
                stats->taken_branches++;
                stats->control_transfers++;
            }
            else
            {
                stats->not_taken_branches++;
            }

            break;

        default:
            break;
    }

    stats->total_cycles +=
        get_instruction_cycles((Opcode)opcode);

    /*
     * Check Watchpoints after execution.
     */

    if (watchpoints != NULL)
    {
        for (size_t i = 0; i < watchpoints->count; i++)
        {
            Watchpoint *wp = &watchpoints->items[i];

            if (!wp->enabled || wp->triggered)
                continue;

            wp->new_value =
                watchpoint_read_value(cpu, wp);

            if (wp->old_value != wp->new_value)
            {
                wp->triggered = TRUE;

                watchpoint_report(wp);
            }
        }
    }

    return TRUE;
}

/* =========================
   Breakpoint Parsing
   ========================= */

static Bool parse_breakpoints(
    const char *input,
    BreakpointSet *breakpoints
)
{
    if (input == NULL || breakpoints == NULL)
        return FALSE;

    char buffer[MAX_PATH_LENGTH];

    size_t length = strlen(input);

    if (length == 0 || length >= sizeof(buffer))
        return FALSE;

    strcpy(buffer, input);

    breakpoints->count = 0;

    char *token = strtok(buffer, ",");

    while (token != NULL)
    {
        char *end = NULL;

        if (token[0] == '-')
            return FALSE;

        unsigned long address = strtoul(token, &end, 0);

        if (end == token || *end != '\0')
            return FALSE;

        if (address >= INSTRUCTION_MEMORY_SIZE)
            return FALSE;

        if (breakpoints->count >= MAX_BREAKPOINTS)
            return FALSE;

        Bool duplicate = FALSE;

        for (size_t i = 0; i < breakpoints->count; i++)
        {
            if (breakpoints->addresses[i] == (word)address)
            {
                duplicate = TRUE;
                break;
            }
        }

        if (!duplicate)
        {
            breakpoints->addresses[breakpoints->count++] =
                (word)address;
        }

        token = strtok(NULL, ",");
    }

    return breakpoints->count > 0;
}

/* =========================
   Breakpoint Check
   ========================= */

static Bool is_breakpoint(
    const BreakpointSet *breakpoints,
    word address
)
{
    if (breakpoints == NULL)
        return FALSE;

    for (size_t i = 0; i < breakpoints->count; i++)
    {
        if (breakpoints->addresses[i] == address)
            return TRUE;
    }

    return FALSE;
}

/* =========================
   Breakpoint Handler
   ========================= */

static void handle_breakpoint(
    Sac16State *cpu,
    const BreakpointSet *breakpoints,
    Bool wait_for_enter
)
{
    if (cpu == NULL || breakpoints == NULL)
        return;

    if (!is_breakpoint(breakpoints, cpu->pc))
        return;

    printf("\n");
    printf("%s", YELLOW);

    printf("================================\n");
    printf("       BREAKPOINT REACHED\n");
    printf("================================\n");

    printf("Address: 0x%04X (%u)\n", cpu->pc, cpu->pc);

    if (cpu->pc < cpu->imem.counter)
    {
        printf("Instruction: 0x%04X\n",
               cpu->imem.instructions[cpu->pc]);

        printf("Assembly:    ");

        disassemble_instruction(
            cpu->imem.instructions[cpu->pc],
            stdout
        );

        printf("\n");
    }

    printf("%s", RESET);

    print_cpu_state(cpu);
    print_data_memory(cpu);

    if (wait_for_enter)
    {
        printf("\nBreakpoint paused. Press ENTER to continue...");
        getchar();
    }
}

/* =========================
   Normal Mode
   ========================= */

static Bool run_normal_mode(
    Sac16State *cpu,
    ExecutionStats *stats,
    const BreakpointSet *breakpoints,
    WatchpointSet *watchpoints
)
{
    printf("\nStarting normal execution...\n");

    size_t step_count = 0;

    while (!cpu->halted)
    {
        if (step_count >= MAX_SIMULATION_STEPS)
        {
            printf(
                "%sWarning: Maximum execution steps reached.%s\n",
                YELLOW,
                RESET
            );

            return TRUE;
        }

        /* Check breakpoint before execution. */

        handle_breakpoint(cpu, breakpoints, TRUE);

        if (!execute_one_instruction(cpu, stats, watchpoints))
        {
            printf(
                "%sCPU Error: %s%s\n",
                RED,
                cpu_error_string(cpu->error),
                RESET
            );

            return FALSE;
        }

        step_count++;

        if (watchpoint_has_triggered(watchpoints))
        {
            printf("\n[WATCHPOINT] Execution paused.\n");
            return TRUE;
        }
    }

    printf(
        "%s[OK] CPU halted successfully.%s\n",
        GREEN,
        RESET
    );

    return TRUE;
}

/* =========================
   Step Mode
   ========================= */

static Bool run_step_mode(
    Sac16State *cpu,
    ExecutionStats *stats,
    const BreakpointSet *breakpoints,
    WatchpointSet *watchpoints
)
{
    printf("\nStarting step-by-step execution...\n");
    printf("Press ENTER to execute the next instruction.\n");
    printf("Press Ctrl+C to stop.\n");

    size_t step_count = 0;

    while (!cpu->halted)
    {
        if (step_count >= MAX_SIMULATION_STEPS)
        {
            printf(
                "%sWarning: Maximum execution steps reached.%s\n",
                YELLOW,
                RESET
            );

            return TRUE;
        }

        if (cpu->pc >= cpu->imem.counter)
        {
            cpu->error = CPU_ERROR_PC_OUT_OF_RANGE;

            printf(
                "%sCPU Error: %s%s\n",
                RED,
                cpu_error_string(cpu->error),
                RESET
            );

            return FALSE;
        }

        /*
         * Step Mode already pauses before every instruction.
         */

        handle_breakpoint(cpu, breakpoints, FALSE);

        word instruction = cpu->imem.instructions[cpu->pc];

        printf("\n");
        printf("================================\n");
        printf("Step %zu\n", step_count + 1);
        printf("================================\n");

        printf("PC:          0x%04X\n", cpu->pc);
        printf("Instruction: 0x%04X\n", instruction);

        printf("Assembly:    ");

        disassemble_instruction(instruction, stdout);

        printf("\n\nPress ENTER...");

        getchar();

        if (!execute_one_instruction(cpu, stats, watchpoints))
        {
            printf(
                "\n%sCPU Error: %s%s\n",
                RED,
                cpu_error_string(cpu->error),
                RESET
            );

            return FALSE;
        }

        print_cpu_state(cpu);
        print_data_memory(cpu);

        step_count++;

        if (watchpoint_has_triggered(watchpoints))
        {
            printf("\n[WATCHPOINT] Execution paused.\n");
            return TRUE;
        }
    }

    printf(
        "\n%s[OK] CPU halted successfully.%s\n",
        GREEN,
        RESET
    );

    return TRUE;
}

/* =========================
   Trace Mode
   ========================= */

static Bool run_trace_mode(
    Sac16State *cpu,
    ExecutionStats *stats,
    const BreakpointSet *breakpoints,
    WatchpointSet *watchpoints
)
{
    printf("\nStarting trace execution...\n");

    word *previous_memory =
        malloc(sizeof(word) * DATA_MEMORY_SIZE);

    if (previous_memory == NULL)
    {
        printf("%sError: Cannot allocate trace memory.%s\n",
               RED,
               RESET);

        return FALSE;
    }

    size_t step_count = 0;

    while (!cpu->halted)
    {
        if (step_count >= MAX_SIMULATION_STEPS)
        {
            printf(
                "%sWarning: Maximum execution steps reached.%s\n",
                YELLOW,
                RESET
            );

            free(previous_memory);
            return TRUE;
        }

        if (cpu->pc >= cpu->imem.counter)
        {
            cpu->error = CPU_ERROR_PC_OUT_OF_RANGE;

            printf(
                "%sCPU Error: %s%s\n",
                RED,
                cpu_error_string(cpu->error),
                RESET
            );

            free(previous_memory);
            return FALSE;
        }

        /* Check breakpoint before execution. */

        handle_breakpoint(cpu, breakpoints, TRUE);

        word instruction = cpu->imem.instructions[cpu->pc];

        printf("\n");
        printf("================================\n");
        printf("Step %zu\n", step_count + 1);
        printf("================================\n");

        printf("PC:          0x%04X\n", cpu->pc);
        printf("Instruction: 0x%04X\n", instruction);

        printf("Assembly:    ");

        disassemble_instruction(instruction, stdout);

        printf("\n");

        memcpy(
            previous_memory,
            cpu->dmem.data,
            sizeof(word) * DATA_MEMORY_SIZE
        );

        if (!execute_one_instruction(cpu, stats, watchpoints))
        {
            printf(
                "%sCPU Error: %s%s\n",
                RED,
                cpu_error_string(cpu->error),
                RESET
            );

            free(previous_memory);
            return FALSE;
        }

        print_cpu_state(cpu);

        print_changed_data_memory(
            previous_memory,
            cpu
        );

        step_count++;

        if (watchpoint_has_triggered(watchpoints))
        {
            printf("\n[WATCHPOINT] Execution paused.\n");

            free(previous_memory);
            return TRUE;
        }
    }

    free(previous_memory);

    printf(
        "\n%s[OK] CPU halted successfully.%s\n",
        GREEN,
        RESET
    );

    return TRUE;
}

/* =========================
   Main
   ========================= */

int main(int argc, char *argv[])
{
    SimulationMode mode = MODE_NORMAL;

    const char *asm_file = NULL;
    const char *breakpoint_input = NULL;

    BreakpointSet breakpoints = {0};
    WatchpointSet watchpoints = {0};

    /*
     * Parse command-line arguments.
     */

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--step") == 0)
        {
            if (mode != MODE_NORMAL)
            {
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }

            mode = MODE_STEP;
        }
        else if (strcmp(argv[i], "--trace") == 0)
        {
            if (mode != MODE_NORMAL)
            {
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }

            mode = MODE_TRACE;
        }
        else if (strcmp(argv[i], "--break") == 0)
        {
            if (breakpoint_input != NULL || i + 1 >= argc)
            {
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }

            breakpoint_input = argv[++i];
        }
        else if (strcmp(argv[i], "--watch") == 0)
        {
            if (i + 1 >= argc)
            {
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }

            unsigned int reg;

            if (!parse_watch_register(argv[++i], &reg))
            {
                printf("Error: Invalid register name.\n");
                return EXIT_FAILURE;
            }

            if (!watchpoint_add(
                    &watchpoints,
                    WATCH_REGISTER,
                    reg))
            {
                printf("Error: Cannot add register Watchpoint.\n");
                return EXIT_FAILURE;
            }
        }
        else if (strcmp(argv[i], "--watch-mem") == 0)
        {
            if (i + 1 >= argc)
            {
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }

            unsigned int address;

            if (!parse_watch_address(argv[++i], &address))
            {
                printf("Error: Invalid memory address.\n");
                return EXIT_FAILURE;
            }

            if (!watchpoint_add(
                    &watchpoints,
                    WATCH_MEMORY,
                    address))
            {
                printf("Error: Cannot add memory Watchpoint.\n");
                return EXIT_FAILURE;
            }
        }
        else if (argv[i][0] == '-')
        {
            printf("Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);

            return EXIT_FAILURE;
        }
        else
        {
            if (asm_file != NULL)
            {
                printf("Error: Multiple assembly files provided.\n");
                return EXIT_FAILURE;
            }

            asm_file = argv[i];
        }
    }

    if (asm_file == NULL)
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    /*
     * Parse breakpoints.
     */

    if (breakpoint_input != NULL)
    {
        if (!parse_breakpoints(breakpoint_input, &breakpoints))
        {
            printf(
                "%sError: Invalid breakpoint addresses.%s\n",
                RED,
                RESET
            );

            printf("Maximum breakpoints: %d\n", MAX_BREAKPOINTS);

            printf("Valid address range: 0 - %d\n",
                   INSTRUCTION_MEMORY_SIZE - 1);

            return EXIT_FAILURE;
        }

        printf("\nBreakpoints:");

        for (size_t i = 0; i < breakpoints.count; i++)
        {
            printf(" 0x%04X", breakpoints.addresses[i]);
        }

        printf("\n");
    }

    /*
     * Print Watchpoints.
     */

    print_watchpoints(&watchpoints);

    /*
     * Run assembler.
     */

    char command[MAX_PATH_LENGTH * 2];

    int command_length = snprintf(
        command,
        sizeof(command),
        "%s \"%s\"",
        ASSEMBLER_PATH,
        asm_file
    );

    if (command_length < 0 ||
        (size_t)command_length >= sizeof(command))
    {
        printf("%sError: Command is too long.%s\n",
               RED,
               RESET);

        return EXIT_FAILURE;
    }

    printf("\n[1] Assembling %s...\n", asm_file);

    int assembler_result = system(command);

    if (assembler_result != 0)
    {
        printf("%sError: Assembler failed.%s\n",
               RED,
               RESET);

        return EXIT_FAILURE;
    }

    printf("%s[OK] Assembly completed.%s\n", GREEN, RESET);

    /*
     * Initialize CPU.
     */

    Sac16State cpu;

    cpu_init(&cpu);

    /*
     * Load Instruction Memory.
     */

    printf("\n[2] Loading Instruction Memory...\n");

    if (!memory_load_instruction(&cpu, BIN_FILE))
    {
        printf("%sError: Cannot load Instruction Memory.%s\n",
               RED,
               RESET);

        return EXIT_FAILURE;
    }

    printf(
        "%s[OK] Loaded %zu instructions.%s\n",
        GREEN,
        cpu.imem.counter,
        RESET
    );

    /*
     * Validate breakpoint addresses.
     */

    for (size_t i = 0; i < breakpoints.count; i++)
    {
        if (breakpoints.addresses[i] >= cpu.imem.counter)
        {
            printf(
                "%sError: Breakpoint 0x%04X is outside the loaded program.%s\n",
                RED,
                breakpoints.addresses[i],
                RESET
            );

            return EXIT_FAILURE;
        }
    }

    /*
     * Load Data Memory.
     */

    printf("\n[3] Loading Data Memory...\n");

    if (!memory_load_data(&cpu, DATA_FILE))
    {
        printf("%sError: Cannot load Data Memory.%s\n",
               RED,
               RESET);

        return EXIT_FAILURE;
    }

    printf(
        "%s[OK] Loaded %zu data words.%s\n",
        GREEN,
        cpu.dmem.counter,
        RESET
    );

    /*
     * Initialize statistics.
     */

    ExecutionStats stats;

    stats_init(&stats);

    /*
     * Initial Memory Status.
     */

    printf("\n================================\n");
    printf("         MEMORY STATUS\n");
    printf("================================\n");

    print_instruction_memory(&cpu);
    print_data_memory(&cpu);

    /*
     * Run selected simulation mode.
     */

    Bool execution_success = FALSE;

    switch (mode)
    {
        case MODE_NORMAL:

            execution_success = run_normal_mode(
                &cpu,
                &stats,
                &breakpoints,
                &watchpoints
            );

            break;

        case MODE_STEP:

            execution_success = run_step_mode(
                &cpu,
                &stats,
                &breakpoints,
                &watchpoints
            );

            break;

        case MODE_TRACE:

            execution_success = run_trace_mode(
                &cpu,
                &stats,
                &breakpoints,
                &watchpoints
            );

            break;

        default:

            printf("%sError: Invalid simulation mode.%s\n",
                   RED,
                   RESET);

            return EXIT_FAILURE;
    }

    /*
     * Final Report.
     */

    printf("\n");
    printf("================================\n");
    printf("         FINAL CPU STATE\n");
    printf("================================\n");

    print_cpu_state(&cpu);

    stats_print(&stats);

    print_instruction_memory(&cpu);
    print_data_memory(&cpu);

    if (!execution_success)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
