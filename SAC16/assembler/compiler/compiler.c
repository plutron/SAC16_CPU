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


Bool parse_args(int argc, char *argv[], char **input, char **output) {
    *input = NULL;
    *output = NULL;
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.asm> -o <output.bin>\n", argv[0]);
        return FALSE;
    }
    
    *input = argv[1];
    
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            *output = argv[i + 1];
            return TRUE;
        }
    }
    
    *output = "a.asm";
    return TRUE;
}


int main(int argc, char *argv[]) {
    char *file_name, *output;
    if (!parse_args(argc, argv, &file_name, &output)) return 1;
    
    printf("Input:  %s\n", file_name);
    printf("Output: %s\n", output);
    
    char base[256];
    strncpy(base, output, sizeof(base) - 1);
    base[sizeof(base) - 1] = '\0';
    char *dot = strrchr(base, '.');
    if (dot) *dot = '\0';
    
    char bin_file[256];
    char data_file[256];
    snprintf(bin_file,  sizeof(bin_file),  "%s.bin",  base);
    snprintf(data_file, sizeof(data_file), "%s.data", base);
    
    printf("Bin:    %s\n", bin_file);
    printf("Data:   %s\n", data_file);
    
    FILE *asm_file = open_file(file_name);
    if (!asm_file) {
        print_error(FileNotFound, file_name);
        return 1;
    }

    Sac16State *sac16 = set_sac16state();
    TypeOfMemory memtyp = None;
    Bool data_check = FALSE;
    Bool text_check = FALSE;
    TypeOfMemory t_mem = None;
    char *line;
    
    while ((line = read_line(asm_file, MAX_SIZE_OF_A_LINE)) != NULL) {
        if (empty_check(line)) {
            free(line);
            continue;
        }
        t_mem = data_text_check(line);
        if (t_mem == Error) {
            free(line);
            goto error_handle;
        }
        if (t_mem == Data && data_check == TRUE) {
            print_error(MoreThenOneTextDataSection, "data");
            free(line);
            goto error_handle;
        }
        if (t_mem == Instruct && text_check == TRUE) {
            print_error(MoreThenOneTextDataSection, "text");
            free(line);
            goto error_handle;
        }
        if (t_mem == Instruct) {
            text_check = TRUE;
            memtyp = Instruct;
            free(line);
            continue;
        }
        if (t_mem == Data) {
            data_check = TRUE;
            memtyp = Data;
            free(line);
            continue;
        }
        if (memtyp == None) {
            print_error(CodeNotStartWithDataText, "");
            free(line);
            goto error_handle;
        }
        
        if (process_line(line, sac16, memtyp)) {
            free(line);
            goto error_handle;
        }
        free(line);
    }
    
    if (make_binary_file(sac16, bin_file, data_file) == FALSE) {
        goto error_handle;
    }
    
    free(sac16);
    fclose(asm_file);
    return 0;
    
error_handle:
    free(sac16);
    fclose(asm_file);
    return 1; 
}