#include <stdio.h>
#include "structs.h"
#include "constants.h"
#include "errors.h"

int update_inst_memory(Sac16State * sac,Instruction * inst){
    if(sac->imem.counter >= INSTRUCTION_MEMORY_SIZE ){
        print_error(MemoryOverflow," instruction memory");
        return 1;}
    sac->imem.instructions[sac->imem.counter++] = *inst;
    return 0;
}


void print_imem(const Sac16State *sac)
{
    printf("\n========== Instruction Memory ==========\n");
    printf("%-6s %-8s %-8s %-8s %-8s %-8s\n",
           "Index", "Address", "Opcode", "Rd", "Rs", "Imm8");

    printf("------------------------------------------------------\n");

    for (size_t i = 0; i < sac->imem.counter; i++) {
        const Instruction *inst = &sac->imem.instructions[i];

        printf("%-6zu 0x%-6X %-8d %-8d %-8d %-8u %-16u\n",
               i,
               (unsigned int)i,
               inst->opcode,
               inst->rd,
               inst->rs,
               inst->imm8,inst->target);

        printf("       Target: %u\n", inst->target);
    }

    printf("========================================\n");
    printf("Total instructions: %zu\n", sac->imem.counter);
}