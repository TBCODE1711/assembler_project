#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"
#include "encoder.h"
#include "symbols_manager.h"
#include "directives.h"


static int handle_command(AsLine* current_line, int* ic);
static int handle_directive(AsLine* current_line, int* dc);



int first_pass(FILE* as_file, int* ic, int* dc)
{
    char line[LINE_LENGTH];
    int line_number = 1;
    AsLine* current_line;
    int error_flag = SUCCESS;

    while(fgets(line, LINE_LENGTH, as_file)){
        int c;
        if (strchr(line, '\n') == NULL && strlen(line) == LINE_LENGTH - 1){
            while ((c = fgetc(as_file)) != '\n' && c != EOF){}
            printf("Error in line %d - line exceeded %d chars\n", line_number, LINE_LENGTH);
            error_flag = ERROR;
            line_number++;
            continue;
        }
            
        current_line = parser(line, line_number);

        switch(current_line->line_type){
            case LINE_COMMAND:
                if(handle_command(current_line, ic) == ERROR){
                error_flag = ERROR;
            }
                break;
            
            case LINE_DIRECTIVE:
                if(handle_directive(current_line, dc) == ERROR){
                error_flag = ERROR;
                }
                break;
                

            case LINE_ERROR:
                error_flag = ERROR;
                break;

            case LINE_COMMENT:
            case LINE_EMPTY:
            case LINE_OK:
            break;
        }

        line_number++;
        free(current_line);
    }
    update_data_address(*ic);
    return error_flag;
}


static int handle_command(AsLine* current_line, int* ic)
{
    if(current_line->defined_lable[0] != '\0'){
        if(search_label(current_line->defined_lable) != NULL){
            printf("Error in line %d - label already exist\n", current_line->line_number);
            return ERROR;
        }
        add_label(current_line->defined_lable, *ic, CODE, LOCAL);
    }

    convert_command(current_line, ic);
    return SUCCESS;
}

static int handle_directive(AsLine* current_line, int* dc)
{
    switch(current_line->line_directive->directive_type){
        case DIR_DB:
        case DIR_DH:
        case DIR_DW:
        case DIR_ASCIZ:
            if(current_line->defined_lable[0] != '\0'){
                if(search_label(current_line->defined_lable) != NULL){
                    printf("Error in line %d - label already exist\n", current_line->line_number);
                    return ERROR;
                }
                add_label(current_line->defined_lable, *dc, DATA, LOCAL);
            } 
            convert_directive(current_line, dc);
            break;

        case DIR_EXTERN:
        {
        Symbol* current_label = search_label(current_line->target_label);

        if(current_label != NULL && current_label->visibility != EXTERN){
                    printf("Error in line %d - label already exist\n", current_line->line_number);
                    return ERROR;
                }
            if(current_label == NULL){    
                add_label(current_line->target_label, 0, NONE, EXTERN);
            }
            break;
        }
        case DIR_ENTRY:
            break;
    }
    return SUCCESS;
}
