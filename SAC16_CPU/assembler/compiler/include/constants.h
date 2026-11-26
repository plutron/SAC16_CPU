#pragma once


//-------------> define
#define MAX_SIZE_OF_ASM_FILE_NAME 1024
#define MAX_SIZE_OF_A_LINE 1024
#define COMMENT_SIGN ';'
#define INSTRUCTION_MEMORY_SIZE 4096
#define DATA_MEMORY_SIZE 65536
#define NUMBER_OF_REGISTERS 8
#define MAX_SIZE_OF_LABEL_NAME 256 // one space for \0
#define SYMBOL_TABLE_SIZE (INSTRUCTION_MEMORY_SIZE + DATA_MEMORY_SIZE)
#define MAX_FILE_NAME_CHAR 256

//----> color
#define RESET       "\033[0m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define BOLD        "\033[1m"
#define BOLD_RED    "\033[1;31m"