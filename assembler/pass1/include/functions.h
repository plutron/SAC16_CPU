#pragma once
#include <stdio.h>

// ----------------> data section
#define MAX_FILE_NAME_CHAR 256


FILE * open_file();
char * input_string(int maxsizeofinput);
char * read_line(FILE *f,int maxsizeofinput);