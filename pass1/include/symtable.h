#pragma once

char *label_check(const char *line,size_t * start_point) ;
void print_symbol_table(const Sac16State *sac) ;
int update_symbol_table(Sac16State *sac,  char *label, TypeOfMemory memtype) ;
