#pragma once

#include "decoder.h"

Bool datapath_execute(Sac16State *cpu,
                      const DecodedInstruction *instruction);