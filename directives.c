#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "directives.h"
#define DIRECTIVE_LENGTH 7


Directive directives_table[] = {
    {".db", 1},
    {".dh", 2},
    {".dw", 4},
    {".asciz", 1} 
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