#pragma once

#include "cpu.h"

// Read a register
Bool register_file_read(
    Sac16State *cpu,
    RegisterName reg,
    word *value
);

// Write a register
Bool register_file_write(
    Sac16State *cpu,
    RegisterName reg,
    word value
);