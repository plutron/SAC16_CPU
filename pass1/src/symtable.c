#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "structs.h"
#include "enums.h"
#include "constants.h"
#include <string.h>

char *label_check(const char *line,size_t * start_point) {
    int i = 0;
    Bool wrong_label = FALSE;
    while (line[i] == ' ' || line[i] == '\t') i++;
    if (line[i] == '\0') return NULL;

    char *label = malloc(MAX_SIZE_OF_LABEL_NAME);
    if (!label) {*start_point = 0; return NULL;}

    int counter = 0;

    while (line[i] != '\0') {
        char c = line[i];
        if (c == ':') {
            if (counter == 0 || wrong_label) {
                print_error(InvalidLabelName, line);
                strncpy(label,"wrong name",MAX_SIZE_OF_LABEL_NAME);
                return label;
            }
            label[counter] = '\0';
            *start_point = i+1;
            return label;
        }

        if (c == ' ' || c == '\t') {
            while((c=line[i]) == ' '|| c == '\t')i++;
            if(c==':'&&!wrong_label){
                label[counter] = '\0';
                *start_point = i+1;
                return label;
            }
            if(c == ':'&&wrong_label){
                print_error(InvalidLabelName, line);
                free(label);
                exit(1);}
            wrong_label = TRUE;
        }

        if (counter == 0) {
            if (!((c >= 'a' && c <= 'z') ||
                  (c >= 'A' && c <= 'Z') ||
                  c == '_')) {
                    wrong_label = TRUE;
            }
        }
        else {
            if (!((c >= 'a' && c <= 'z') ||
                  (c >= 'A' && c <= 'Z') ||
                  (c >= '0' && c <= '9') ||
                  c == '_')) {
                    wrong_label = TRUE;
            }
        }
        if (counter >= MAX_SIZE_OF_LABEL_NAME - 1) {
            print_error(InvalidLabelName, "label too long");
            strncpy(label,"wrong name",MAX_SIZE_OF_LABEL_NAME);
            *start_point = 0;
            return label;
        }

        label[counter++] = c;
        i++;
    }
    free(label);
    *start_point = 0;
    return NULL;
}

void print_symbol_table(const Sac16State *sac) {
    printf("\n=== SYMBOL TABLE (Count: %d) ===\n", sac->sytable_counter);
    printf("%-5s | %-20s | %-8s | %-10s\n", "Index", "Label Name", "Address", "Type");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < sac->sytable_counter; i++) {
        // تبدیل نوع حافظه به رشته برای خوانایی
        const char *type_str = (sac->sytable[i].type == Instruct) ? "INSTRUCT" : "DATA";
        
        printf("%-5d | %-20s | 0x%04X   | %-10s\n", 
               i,
               sac->sytable[i].label,
               sac->sytable[i].address,
               type_str);
    }
    printf("==================================================\n\n");

}

int update_symbol_table(Sac16State *sac,  char *label, TypeOfMemory memtype) {

    if (sac->sytable_counter >= SYMBOL_TABLE_SIZE) {
        
        print_error(MemoryOverflow, "Symbol table is full");
        return 1;
    }
    for(size_t i = 0 ; i < sac->sytable_counter ; i++){
        if(!strcmp(label,sac->sytable[i].label))
        {print_error(DuplicateLabel,label);return 1;}
    }

    strncpy(sac->sytable[sac->sytable_counter].label, label, MAX_SIZE_OF_LABEL_NAME - 1);
    
    sac->sytable[sac->sytable_counter].label[MAX_SIZE_OF_LABEL_NAME - 1] = '\0';
    if(memtype == Instruct)
        sac->sytable[sac->sytable_counter].address = sac->imem.counter;
    else
        sac->sytable[sac->sytable_counter].address = sac->dmem.counter;
    sac->sytable[sac->sytable_counter].type = memtype;
    sac->sytable_counter++;
    return 0;
}


word change_label_to_address(Sac16State *sac, const char *label, TypeOfMemory type) {
    for (int i = 0; i < sac->sytable_counter; i++) {
        if (strcasecmp(sac->sytable[i].label, label) == 0) {
            if (sac->sytable[i].type != type) {
                return (word)-1; 
            }
            return sac->sytable[i].address;  
        }
    }
    return (word)-1;
}