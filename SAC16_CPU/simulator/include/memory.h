#pragma once

#include "cpu.h"

//-------------> Instruction Memory
Bool memory_read_instruction(Sac16State *cpu, word address, word *result);

Bool memory_load_instruction(Sac16State *cpu, const char *filename);


//-------------> Data Memory
Bool memory_read_data(Sac16State *cpu, word address, word *result);

Bool memory_write_data(Sac16State *cpu, word address, word value);

Bool memory_load_data(Sac16State *cpu, const char *filename);