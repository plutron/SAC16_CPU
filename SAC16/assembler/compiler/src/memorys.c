#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "structs.h"
#include "enums.h"
#include "constants.h"
#include <string.h>
#include <strings.h>



TypeOfMemory data_text_check(const char *line){
    char c;
    size_t i = 0 ;
    size_t counter = 0;
    char * text = malloc (sizeof(char) * MAX_SIZE_OF_A_LINE);
    while((c=line[i])==' '|| c == '\t')i++;
    if(c=='.'){

        i++;
        while((c=line[i])==' '|| c =='\t')i++;
        while(c!=' '&&c!='\t'&&c!='\0'&&c!=EOF){
                *(text + counter) = c;

                i++;
                counter ++;
                c = line[i];
        }
        *(text + counter) = '\0';
        if (!strcasecmp(text, "text") || !strcasecmp(text, "data")) {
            int is_text = !strcasecmp(text, "text");
            i++;
            while ((c = line[i]) != '\0' && c != '\n') {
                if (c != ' ' && c != '\t') {
                    print_error(LineNotValid, line);
                    free(text);
                    return Error;
                }
                i++;
            }

            free(text);
            return is_text ? Instruct : Data;
        }
    }    
    free(text);
    return None;

}

