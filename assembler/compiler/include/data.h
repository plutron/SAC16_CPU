#pragma once
#include "structs.h"

word str_to_int(const char *s) ;
int data_chek(char *line, size_t start_point, word *data1);
int update_data_memory(Sac16State * sac,word data);
void print_dmem(const Sac16State *sac);
int check_format(const char *s);