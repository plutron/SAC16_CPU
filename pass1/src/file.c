#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "errors.h"
#include "line.h"
#include "constants.h" 


FILE * open_file(char file_name[]){
    FILE * file = fopen(file_name,"r");
    if(file  == NULL){
        print_error(FileNotFound,file_name);
    }
    return file;
}
