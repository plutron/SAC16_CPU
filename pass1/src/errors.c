#include <stdio.h>
#include "constants.h"
#include "enums.h"


void print_error(ErrorType code, char *text) {
    if (code == FileNotFound) {
        printf(RED "FILE NOT FOUND: %s\n" RESET, text);
    }
    if(code == NotEnughSpace){
       printf(RED "Not enugh sapce in 8 bit %s\n" RESET, text);     
    }
    else if( code == InvalidDataLine){
       printf(RED "Invalid data line: %s\n" RESET, text);     
    }
    else if ( code == NotEnughReg){
        printf(RED "Not enugh operand: %s\n" RESET, text);
    }
    else if(code == LineNotValid){
        printf(RED "Line not valid: %s\n" RESET, text);
    }
    else if (code == InvalidLabelName) {
        printf(RED "Invalid label name: %s\n" RESET, text);
    }

    else if (code == FileNameTooLong) {
        printf(RED "File name too long: %s\n" RESET, text);
    }
    else if (code == InvalidRegister) {
        printf(RED "Invalid register: %s\n" RESET, text);
    }
    else if (code == InvalidOpcode) {
        printf(RED "Invalid opcode: %s\n" RESET, text);
    }
    else if (code == InvalidOperand) {
        printf(RED "Invalid operand: %s\n" RESET, text);
    }
    else if (code == DuplicateLabel) {
        printf(RED "Duplicate label: %s\n" RESET, text);
    }
    else if (code == UnknownLabel) {
        printf(RED "Unknown label: %s\n" RESET, text);
    }
    else if (code == InvalidImmediate) {
        printf(RED "Invalid immediate: %s\n" RESET, text);
    }
    else if (code == InvalidTarget) {
        printf(RED "Invalid target address: %s\n" RESET, text);
    }
    else if (code == MemoryOverflow) {
        printf(RED "Memory overflow: %s\n" RESET, text);
    }
    else if(code == TooManyOperand){
        printf(RED "Too many operand: %s\n" RESET, text);
    }
    else if(code == MoreThenOneTextDataSection){
                printf(RED "More then one sectoin of : %s\n" RESET, text);
    }
    else if(code == CodeNotStartWithDataText){
                printf(RED "Code dose not start with text or data %s\n" RESET, text);
    }
    
    else {
        printf(RED "Unknown error (code %d): %s\n" RESET, (int)code, text);
    }
}