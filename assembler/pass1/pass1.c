#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "functions.h"



int main(){
    FILE * asm_file = open_file();
    char * line ;

    while((line = read_line(asm_file,MAX_SIZE_OF_A_LINE))!=NULL){
        
    }
    free(line);


    fclose(asm_file);
}