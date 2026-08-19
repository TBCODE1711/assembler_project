#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "directives.h"


Directive directives_table[] = {
    {".db", 1, DIR_DB},
    {".dh", 2, DIR_DH},
    {".dw", 4, DIR_DW},
    {".asciz", 1, DIR_ASCIZ},
    {".entry", 0, DIR_ENTRY},
    {".extern", 0, DIR_EXTERN}
};


Directive* search_directive(char* search_name)
{
    int i;

    for(i = 0; i < (sizeof(directives_table) / sizeof(directives_table[0])); i++)
    {
        if(strcmp(directives_table[i].directive_name, search_name) == 0){
            return &(directives_table[i]);
        }
    }
    return NULL;
}