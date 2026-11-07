#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "functions.h"
#include "errors.h"
#include "structs.h"
#include "hardware.h"
#include "io.h"
#include <string.h> 


Bool empty_check(const char * line){
    if(*(line) == '\0') return TRUE;
    return FALSE;
}

Sac16State *set_sac16state() {
    Sac16State *state = calloc(1, sizeof(Sac16State));
    
    if (state == NULL) {
        fprintf(stderr, "Fatal Error: Cannot allocate memory for Sac16State\n");
        return NULL;
    }

    state->imem.counter = 0; 
    state->dmem.counter = 0;
    return state; 
}