#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "file.h"
#include "structs.h"
#include "functions.h"
#include "errors.h"
#include "memorys.h"
#include "line.h"
#include "io.h"
#include "symtable.h"
#include "data.h"
#include "imemory.h"


int main(){
    //   ---- > variables 
    char * file_name = "a.txt";   
    FILE * asm_file;
    char * line ;
    Sac16State * sac16 =  set_sac16state();
    TypeOfMemory memtyp = None;
    Bool data_check = FALSE; // this two use for showing if data ot text part is shon in code
    Bool text_check = FALSE;
    TypeOfMemory t_mem = None;
    //   ----- > open file //done
    open_file_start : 
    printf("input file name : ");
    //file_name = input_string(MAX_SIZE_OF_A_LINE);
    asm_file = open_file(file_name);
    //free(file_name);
    if(!asm_file) goto open_file_start;

    //   ------ > line process
    while((line = read_line(asm_file,MAX_SIZE_OF_A_LINE))!=NULL){
            
        if(empty_check(line)){free(line);continue;} //done
        if((t_mem = data_text_check(line)) != None){ // done
            if(t_mem == Error) goto error_handle;
            if(t_mem == Data && data_check == TRUE){
                print_error(MoreThenOneTextDataSection,"data");
                goto error_handle;
            }
            if(t_mem == Instruct && text_check == TRUE){
                print_error(MoreThenOneTextDataSection,"text");
                goto error_handle;
            }
            if(t_mem == Instruct){
                text_check = TRUE;
                memtyp = Instruct;
            continue;}
            else if (t_mem == Data){
                data_check = TRUE;
                memtyp = Data;
            continue;}
        }
        if(memtyp == None){ //done
            print_error(CodeNotStartWithDataText,"");
            goto error_handle;
        }
        if(process_line(line,sac16,memtyp)){
            goto error_handle;
        }
        free(line);
    }


    print_imem(sac16);
    print_dmem(sac16);
    print_symbol_table(sac16);

    error_handle:

    free(sac16);
    fclose(asm_file);
}