#pragma once
#include <stdio.h>
#include "enums.h"
#include "structs.h"



FILE * open_file(char file_name[]);
Bool make_binary_file(Sac16State * sac,char * inst_name,char * data_name);