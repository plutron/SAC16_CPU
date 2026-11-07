#include <stdio.h>
#include "structs.h"
#include "enums.h"
#include "symtable.h"
#include "memorys.h"
#include "instructions.h"
#include "imemory.h"
#include "data.h"

Bool process_line(const char * line,Sac16State * sac,TypeOfMemory memtyp){
    char * lable_name;
    word data;
    size_t  start_point ;
    Instruction * inst ;
    if((lable_name = label_check(line,&start_point)) != NULL){
        if(!strcmp(lable_name,"wrong name")){
            goto error_handle;}
        
        if(update_symbol_table(sac,lable_name,memtyp)){
        goto error_handle;}
    }
    if(memtyp == Instruct){

        if((inst=inst_chek(line,start_point)) == NULL)
            {free(inst);goto error_handle;}
        else if(inst->opcode != EMPTY){
            if(update_inst_memory(sac,inst))
              {free(inst);goto error_handle;}
            }
        free(inst);
        }
    else if(memtyp == Data){

        if(data_chek(line,start_point,&data))
            goto error_handle;
        
        if(update_data_memory(sac,data))
            goto error_handle;
        }

    else{

    }

    free(lable_name);
    return 0;
    error_handle:

    free(lable_name);
    return 1;
}

