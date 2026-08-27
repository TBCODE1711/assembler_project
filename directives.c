#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "directives.h"

#define DB_SIZE 1
#define DH_SIZE 2
#define DW_SIZE 4
#define ASCIZ_CHAR_SIZE 1
#define NO_DATA_SIZE 0


Directive directives_table[] = {
    {".db", DB_SIZE, DIR_DB},
    {".dh", DH_SIZE, DIR_DH},
    {".dw", DW_SIZE, DIR_DW},
    {".asciz", ASCIZ_CHAR_SIZE, DIR_ASCIZ},
    {".entry", NO_DATA_SIZE, DIR_ENTRY},
    {".extern", NO_DATA_SIZE, DIR_EXTERN}
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