#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "errors.h"
#include "line.h"
#include "constants.h" 
#include "instructions.h"
#include "symtable.h"
#include "string.h"
#include "data.h"

FILE * open_file(char file_name[]){
    FILE * file = fopen(file_name,"r");
    if(file  == NULL){
        print_error(FileNotFound,file_name);
    }
    return file;
}

word R_format(Sac16State *sac, int i) {
    word bits = 0;
    Instruction *inst = &sac->imem.instructions[i];

    bits |= ((word)(inst->opcode & 0xF) << 12);
    bits |= ((word)(inst->rd & 0xF) << 8);
    bits |= ((word)(inst->rs & 0xF) << 4);
    
    return bits;
}

word N_format(Sac16State *sac, int i) {
    word bits = 0;
    Instruction *inst = &sac->imem.instructions[i];

    bits |= ((word)(inst->opcode & 0xF) << 12);
    
    return bits;
}

word LDI_label(Sac16State *sac, int i) {
    word bits = 0;
    Instruction *inst = &sac->imem.instructions[i];
    
    if (strcmp(inst->target, "") != 0) {
        word c = change_label_to_address(sac, inst->target, Data);

        if (c == (word)-1) {
            print_error(InvalidLabelName, inst->target);
            return (word)-1;
        }
        if (c > 255) {
            print_error(NotEnughSpace, inst->target);
            return (word)-1;
        }
        
        inst->imm8 = (uint8_t)c;
    }
    
    bits |= ((word)(inst->opcode & 0xF) << 12);
    bits |= ((word)(inst->rd     & 0xF) << 8);
    bits |= ((word)(inst->imm8   & 0xFF));
    
    return bits;
}

word JMP_label(Sac16State *sac, int i) {
    word bits = 0;
    Instruction *inst = &sac->imem.instructions[i];
    word addr = 0;
    
    if (strcmp(inst->target, "") != 0) {
        if (check_format(inst->target) == 0) {
            long val = strtol(inst->target, NULL, 0);
            if (val < 0 || val > 4095) {
                print_error(InvalidTarget, inst->target);
                return (word)-1;
            }
            addr = (word)val;
        } else {
            word c = change_label_to_address(sac, inst->target, Instruct);
            if (c == (word)-1) {
                print_error(InvalidLabelName, inst->target);
                return (word)-1;
            }
            if (c > 4095) {
                print_error(NotEnughSpace, inst->target);
                return (word)-1;
            }
            addr = c;
        }
    }
    
    bits |= ((word)(inst->opcode & 0xF) << 12);
    bits |= (addr & 0xFFF);
    
    return bits;
}

Bool make_binary_file(Sac16State *sac, char *inst_name, char *data_name) {
    FILE *fi = fopen(inst_name, "wb");
    if (!fi) {
        print_error(FileNotFound, inst_name);
        return FALSE;
    }
    FILE *fd = fopen(data_name, "wb");
    if (!fd) {
        print_error(FileNotFound, data_name);
        fclose(fi);
        return FALSE;
    }
    for (int i = 0; i < sac->imem.counter; i++) {
        word encoded = 0;
        Opcode op = sac->imem.instructions[i].opcode;
        
        switch (op) {
            case LDI:
                encoded = LDI_label(sac, i);
                break;

            case JMP:
            case JZ:
            case JNZ:
            case CALL:
                encoded = JMP_label(sac, i);
                break;

            case LD:
            case ST:
            case MOV:
            case ADD:
            case SUB:
            case MUL:
            case AND:
            case OR:
                encoded = R_format(sac, i);
                break;

            case NOT:
            case RET:
            case HALT:
                encoded = N_format(sac, i);
                break;
            
            default:
                print_error(InvalidOpcode, "");
                fclose(fi);
                fclose(fd);
                return FALSE;
        }

        if (encoded == (word)-1) {
            fclose(fi);
            fclose(fd);
            return FALSE;
        }

        fputc((encoded >> 8) & 0xFF, fi);
        fputc(encoded & 0xFF, fi);
    }

    for (int i = 0; i < sac->dmem.counter; i++) {
            uint16_t data12 = sac->dmem.data[i];

            fputc((data12 >> 8) & 0xFFFF, fd);
            fputc(data12 & 0xFF, fd);

        
    }
    
    fclose(fi);
    fclose(fd);
    
    printf("✅ Generated %s (%d instructions) and %s\n",
           inst_name, sac->imem.counter, data_name);
    
    return TRUE;
}