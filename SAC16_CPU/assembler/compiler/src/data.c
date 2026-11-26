#include <stdio.h>
#include "structs.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "errors.h"

int check_format(const char *s)
{
    if (!s) return 1;

    while (*s == ' ' || *s == '\t' || *s == '\n' ||
           *s == '\r' || *s == '\f' || *s == '\v')
        s++;

    if (*s == '+' || *s == '-')
        s++;

    int base = 10;

    if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B')) {
        base = 2;
        s += 2;
    } else if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        base = 16;
        s += 2;
    } else if (s[0] == '0' && s[1] != '\0') {
        base = 8;
        s++;
    }

    int digits = 0;

    while (*s) {
        int ok = 0;

        if (base == 2)
            ok = (*s == '0' || *s == '1');
        else if (base == 8)
            ok = (*s >= '0' && *s <= '7');
        else if (base == 10)
            ok = (*s >= '0' && *s <= '9');
        else if (base == 16)
            ok = (*s >= '0' && *s <= '9') ||
                 (*s >= 'a' && *s <= 'f') ||
                 (*s >= 'A' && *s <= 'F');

        if (!ok)
            break;

        digits = 1;
        s++;
    }

    if (!digits)
        return 1;

    while (*s == ' ' || *s == '\t' || *s == '\n' ||
           *s == '\r' || *s == '\f' || *s == '\v')
        s++;

    return (*s == '\0') ? 0 : 1;
}

word str_to_int(const char *s) {
    int neg = 0;
    if (*s == '-') { neg = 1; s++; }
    else if (*s == '+') s++;
    int base = 10;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) { base = 16; s += 2; }
    else if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B')) { base = 2; s += 2; }
    else if (s[0] == '0' && s[1] != '\0') { base = 8; s++; }
    word val = (word)strtol(s, NULL, base);
    return neg ? -val : val;
}


int data_chek(char *line, size_t start_point, word *data1)
{
    size_t i = start_point;
    Bool flag = FALSE;

    size_t counter = 0;
    word data = 0;
    char *text = malloc(sizeof(char) * MAX_SIZE_OF_A_LINE);
    char c;
    size_t number_of_inputs = 0;
    long check_val;
    char *nump;
    int neg, base;

    if (!text)
        return 1;

    while (1) {
        counter = 0;

        while ((c = line[i]) == ' ' || c == '\t' || c == '\r')
            i++;

        if (c == '\n' || c == '\0' || c == EOF)
            break;

        while (c != ' ' && c != '\t' && c != '\n' && c != '\0' &&
               c != EOF) {
            if (counter + 1 < MAX_SIZE_OF_A_LINE)
                text[counter++] = c;
            c = line[++i];
        }
        text[counter] = '\0';

        if (number_of_inputs == 0) {
            if (strcasecmp(text, ".word") != 0) {
                print_error(InvalidDataLine, line);
                goto error_handle;
            }flag = TRUE;
        } else {
            if (check_format(text)) {
                print_error(InvalidDataLine, line);
                goto error_handle;
            }

            nump = text;
            neg = 0;
            if (*nump == '+' || *nump == '-') { neg = (*nump == '-'); nump++; }
            base = 10;
            if (nump[0] == '0' && (nump[1] == 'x' || nump[1] == 'X')) { base = 16; nump += 2; }
            else if (nump[0] == '0' && (nump[1] == 'b' || nump[1] == 'B')) { base = 2; nump += 2; }
            else if (nump[0] == '0' && nump[1] != '\0') { base = 8; nump++; }

            check_val = strtol(nump, NULL, base);
            if (neg) check_val = -check_val;

            data = str_to_int(text);

            if ((long)data != check_val) {
                print_error(InvalidDataLine, line);
                goto error_handle;
            }

            *data1 = data;
        }
        number_of_inputs++;
    }

    if (number_of_inputs < 2 && flag) {
        print_error(InvalidDataLine, line);
        goto error_handle;
    }

    free(text);
    return 0;

error_handle:
    free(text);
    return 1;
}


int update_data_memory(Sac16State * sac,word data){
    if(sac->dmem.counter >= DATA_MEMORY_SIZE) return 1;
    sac->dmem.data[sac->dmem.counter++] = data;
    return 0;

}


void print_dmem(const Sac16State *sac)
{
    printf("\n========== Data Memory ==========\n");
    printf("%-6s %-10s %-8s %-8s\n",
           "Index", "Address", "Value(dec)", "Value(hex)");

    printf("--------------------------------------------\n");

    for (size_t i = 0; i < sac->dmem.counter; i++) {
        word v = sac->dmem.data[i];

        printf("%-6zu 0x%-8X %-10u 0x%-6X\n",
               i,
               (unsigned int)i,
               (unsigned int)v,
               (unsigned int)v);
    }

    printf("============================================\n");
    printf("Total words: %zu\n", sac->dmem.counter);
}