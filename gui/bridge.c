#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"
#include "memory.h"
#include "disassembler.h"

typedef struct { Sac16State cpu; } GuiCPU;

void *sac_create(void) {
    GuiCPU *g = (GuiCPU *)calloc(1, sizeof(GuiCPU));
    if (!g) return NULL;
    cpu_init(&g->cpu);
    return g;
}
void sac_destroy(void *handle) { free(handle); }
int sac_load(void *handle, const char *imem_path, const char *dmem_path) {
    if (!handle || !imem_path || !dmem_path) return 0;
    GuiCPU *g = (GuiCPU *)handle;
    cpu_init(&g->cpu);
    if (!memory_load_instruction(&g->cpu, imem_path)) return 0;
    if (!memory_load_data(&g->cpu, dmem_path)) return 0;
    return 1;
}
int sac_step(void *handle) { return handle && cpu_step(&((GuiCPU *)handle)->cpu) ? 1 : 0; }
void sac_reset(void *handle) { if (handle) cpu_reset(&((GuiCPU *)handle)->cpu); }
uint16_t sac_pc(void *h) { return h ? ((GuiCPU *)h)->cpu.pc : 0; }
uint16_t sac_sp(void *h) { return h ? ((GuiCPU *)h)->cpu.sp : 0; }
uint16_t sac_acc(void *h) { return h ? ((GuiCPU *)h)->cpu.regs.ACC : 0; }
uint16_t sac_reg(void *h, int n) {
    if (!h || n < 0 || n >= NUMBER_OF_REGISTERS) return 0;
    return ((GuiCPU *)h)->cpu.regs.R[n];
}
int sac_zero(void *h) { return h ? ((GuiCPU *)h)->cpu.flags.zero : 0; }
int sac_carry(void *h) { return h ? ((GuiCPU *)h)->cpu.flags.carry : 0; }
int sac_halted(void *h) { return h ? ((GuiCPU *)h)->cpu.halted : 0; }
int sac_error(void *h) { return h ? ((GuiCPU *)h)->cpu.error : -1; }
const char *sac_error_text(void *h) {
    return h ? cpu_error_string(((GuiCPU *)h)->cpu.error) : "Invalid CPU";
}
size_t sac_imem_count(void *h) { return h ? ((GuiCPU *)h)->cpu.imem.counter : 0; }
size_t sac_dmem_count(void *h) { return h ? ((GuiCPU *)h)->cpu.dmem.counter : 0; }
uint16_t sac_imem_at(void *h, size_t n) {
    return (!h || n >= INSTRUCTION_MEMORY_SIZE) ? 0 : ((GuiCPU *)h)->cpu.imem.instructions[n];
}
uint16_t sac_dmem_at(void *h, size_t n) {
    return (!h || n >= DATA_MEMORY_SIZE) ? 0 : ((GuiCPU *)h)->cpu.dmem.data[n];
}
int sac_disassemble(uint16_t instruction, char *buffer, size_t size) {
    if (!buffer || size == 0) return 0;
    FILE *f = tmpfile();
    if (!f) return 0;
    if (!disassemble_instruction(instruction, f)) { fclose(f); return 0; }
    rewind(f);
    size_t n = fread(buffer, 1, size - 1, f);
    buffer[n] = '\0';
    fclose(f);
    return 1;
}
