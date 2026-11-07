#pragma once

#include <stdio.h>
#include "decoder.h"

//-------------> Disassembler
Bool disassemble_instruction(word instruction, FILE *output);
Bool decoder(
    const char *input_filename,
    const char *output_filename
);
