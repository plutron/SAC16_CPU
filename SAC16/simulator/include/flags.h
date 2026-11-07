#pragma once

#include "cpu.h"
#include "alu.h"

void flags_update(Sac16State *cpu, const ALUResult *result);
void flags_reset(Sac16State *cpu);