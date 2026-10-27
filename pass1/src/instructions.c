#include <stdio.h>
#include "enums.h"
#include "structs.h"
#include "errors.h"
#include <stdlib.h>
#include <string.h>
#include "data.h"

int num_of_reg(Opcode opcode) {
    switch (opcode) {
        case LD:
        case ST:
        case MOV:
        case LDI:
            return 2;

        case ADD:
        case SUB:
        case MUL:
        case AND:
        case OR:
        case NOT:
        case JMP:
        case JZ:
        case JNZ:
        case CALL:
            return 1;



        case RET:
        case HALT:
            return 0;

        default:
            return -1;
    }
}

Opcode opcode_check(const char *text) {
    if (strcasecmp(text, "LD") == 0)   return LD;
    if (strcasecmp(text, "ST") == 0)   return ST;
    if (strcasecmp(text, "MOV") == 0)  return MOV;
    if (strcasecmp(text, "ADD") == 0)  return ADD;
    if (strcasecmp(text, "SUB") == 0)  return SUB;
    if (strcasecmp(text, "MUL") == 0)  return MUL;
    if (strcasecmp(text, "AND") == 0)  return AND;
    if (strcasecmp(text, "OR") == 0)   return OR;
    if (strcasecmp(text, "NOT") == 0)  return NOT;
    if (strcasecmp(text, "JMP") == 0)  return JMP;
    if (strcasecmp(text, "JZ") == 0)   return JZ;
    if (strcasecmp(text, "JNZ") == 0)  return JNZ;
    if (strcasecmp(text, "CALL") == 0) return CALL;
    if (strcasecmp(text, "RET") == 0)  return RET;
    if (strcasecmp(text, "LDI") == 0)  return LDI;
    if (strcasecmp(text, "HALT") == 0) return HALT;
    
    return INVALID_OPCODE; 
}

RegisterName get_register(const char *text) {
    if (strcasecmp(text, "R0") == 0)  return R0;
    if (strcasecmp(text, "R1") == 0)  return R1;
    if (strcasecmp(text, "R2") == 0)  return R2;
    if (strcasecmp(text, "R3") == 0)  return R3;
    if (strcasecmp(text, "R4") == 0)  return R4;
    if (strcasecmp(text, "R5") == 0)  return R5;
    if (strcasecmp(text, "R6") == 0)  return R6;
    if (strcasecmp(text, "R7") == 0)  return R7;
    if (strcasecmp(text, "ACC") == 0) return ACC;
    
    return INVALID_REG;
}

Instruction * inst_chek(const char *line,size_t start_point) {
    size_t i=start_point;
    size_t counter = 0;
    Instruction * inst = malloc(sizeof(Instruction));
    inst->opcode = EMPTY;
    char * text = malloc(sizeof(char) * MAX_SIZE_OF_A_LINE);
    char c;
    size_t max_number_of_reg = 0;
    size_t number_of_inputs = 0;
    while((c = line[i]) == ' ' || c == '\t')i++;
    while(1){
        if(c == ' '|| c == '\t' || c == ','||c=='\n'||c==EOF||c=='\0'){
            if(counter == 0 ){return inst;}
            number_of_inputs ++;
            *(text + counter) = '\0';
            counter = 0;
            c = line[++i];
            if(number_of_inputs == 1){
                inst->opcode = opcode_check(text);
                if(inst->opcode == INVALID_OPCODE){
                    print_error(InvalidOpcode,text);
                    goto error_handle;
                }
                max_number_of_reg = num_of_reg(inst->opcode);
                while(c==' '||c == '\t'||c==',')c=line[++i];
                continue;
            }
            else if(number_of_inputs > max_number_of_reg +1){
                print_error(TooManyOperand,line);
                goto error_handle;
            }
            else if(number_of_inputs == 2){
                if(inst->opcode == JMP || inst->opcode == JZ
                    || inst->opcode == JNZ || inst->opcode == CALL
                ) {
                    if(check_format(text)){print_error(InvalidOperand,text);
                    goto error_handle;}
                    inst->target = str_to_int(text);
                }
                else {inst->rd = get_register(text);
                if(inst->rd ==  INVALID_REG){
                    print_error(InvalidRegister,text);
                    goto error_handle;
                }}
                while(c==' '||c == '\t'||c==',')c=line[++i];
            }
            else if(number_of_inputs == 3){
                if(inst->opcode == LDI) {
                    if(check_format(text)){print_error(InvalidOperand,text);
                    goto error_handle;}
                    inst->imm8 = str_to_int(text);
                }
                else{
                inst->rs = get_register(text);
                if(inst->rs ==  INVALID_REG){
                    print_error(InvalidRegister,text);
                    goto error_handle;
                }}
                while(c==' '||c == '\t'||c==',')c=line[++i];
            }
            if(c == '\0'||c==EOF||c=='\n')break;
        }

        *(text + counter ++)  = c;
        c = line[++i];

    }

    if(max_number_of_reg+1 != number_of_inputs){print_error(NotEnughReg,line); goto error_handle;}
    return inst;
    error_handle:
    free(inst);
    free(text);
    return NULL;
}