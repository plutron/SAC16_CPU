#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "structs.h"
#include "enums.h"
#include "constants.h"
#include <string.h>

void update_memory(Sac16State * sac,char * inst_or_data,word address,TypeOfMemory memtyp);
Instruction * inst_chek(const char *line,size_t start_point) ;
TypeOfMemory data_text_check(char *line);
int num_of_reg(const char *text) ;
