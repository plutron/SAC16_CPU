#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "structs.h"
#include "enums.h"
#include "errors.h"

char * input_string(int maxsizeofinput){
    char * text = malloc(sizeof(char)* maxsizeofinput);
    int counter = 0 ;
    char c;
    while((c=getchar())== ' '||c=='\t');
    while(c!='\n'&&c!=' '&&c!='\t'){
        if(counter >= maxsizeofinput-1){
            free(text);
            print_error(FileNameTooLong,"");
            exit(1); 
        }

        *(text + counter) = c;
        counter ++;
        c=getchar();
    }
    *(text + counter) =  '\0';
    return text;
}

char *read_line(FILE *f, int maxsizeofinput) {
    if (!f || maxsizeofinput <= 0) return NULL;

    char *text = malloc(sizeof(char) * maxsizeofinput);
    if (!text) return NULL;

    int counter = 0;
    char c = fgetc(f);

    if (c == EOF) {
        free(text);
        return NULL; 
    }

    while (c == ' ' || c == '\t') c = fgetc(f);

    while (c != '\n' && c != EOF) {
        if (c == COMMENT_SIGN) {
            while ((c = fgetc(f)) != COMMENT_SIGN && c != '\n' && c != EOF);

            if (c == COMMENT_SIGN) {
                text[counter] = ' ';
                counter++;
                c = fgetc(f);
                continue; 
            } else {
                text[counter] = '\0';
                return text;
            }
        }

        if (counter >= maxsizeofinput - 1) {
            print_error(TooManyCharInLine, "Line exceeds max length"); 
            free(text); 
            return NULL;
        }
        text[counter] = c;
        counter++;
        c = fgetc(f);
    }

    text[counter] = '\0';
    return text;
}