#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "functions.h"
#include "errors.h"


char * input_string(int maxsizeofinput){
    char * text = malloc(sizeof(char)* maxsizeofinput);
    int counter = 0 ;
    char c;
    while((c=getchar())== ' '||c=='\t');
    while(c!='\n'&&c!=' '&&c!='\t'){
        if(counter >= maxsizeofinput-1){
            free(text);
            printf(RED "input out of range, max size is :" "%d" " char\n"  RESET,maxsizeofinput);
            exit(1); 
        }

        *(text + counter) = c;
        counter ++;
        c=getchar();
    }
    *(text + counter) =  '\0';
    return text;
}

char * read_line(FILE *f,int maxsizeofinput){
    char * text = malloc(sizeof(char)* maxsizeofinput);
    int counter = 0 ;
    char c=fgetc(f);
    if(c == EOF) return NULL;
    while(c== ' '||c=='\t')c=fgetc(f);
    while(c!='\n'&&c!=EOF){
        if(c == COMMENT_SIGN){
            while((c=fgetc(f))!=COMMENT_SIGN && c!= '\n' && c!= EOF);
            if(c == COMMENT_SIGN) {*(text + counter) = ' ';counter++;c=fgetc(f); continue; } // if add;comment;t1 it can have error so we add a sapce for it
            else{ 
                *(text + counter) = '\0';
                return text;
            }
        }
        
            if(counter >= maxsizeofinput-1){
            
        }

        *(text + counter) = c;
        counter ++;
        c=fgetc(f);
    }
    *(text + counter) =  '\0';
    return text;
}

FILE * open_file(){
    start:
    printf("input file name : ");
    char * file_name = input_string(MAX_FILE_NAME_CHAR);
    FILE * file = fopen(file_name,"r");
    if(file  == NULL){
        free(file_name);
        print_error(FileNotFound);
        goto start;
    }
    free(file_name);
    return file;
}

