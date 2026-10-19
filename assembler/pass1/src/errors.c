#include <stdio.h>
#include "constants.h"
#include "enums.h"


void print_error(ErrorType code){
    if (code == FileNotFound){ // file not found
        printf(RED "FILE NOT FOUND!\n" RESET);
    }

}