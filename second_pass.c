#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "second_pass.h"
#include "globals.h"
#include "parser.h"
#include "symbols_manager.h"
#include "encoder.h"
#include "directives.h"

static int update_label(AsLine* current_line, int* ic, ExternNode** extern_list);
static int update_label(AsLine* current_line, int* ic, ExternNode** extern_list);
static int update_extern(Symbol* label, int ic, ExternNode** extern_list, int line_number);
static int handel_entry(AsLine* current_line);


int second_pass(FILE* as_file, int* ic, ExternNode** extern_list)
{
    AsLine* current_line;
    int line_number = 1;
    char line[LINE_LENGTH];
    int error_flag = SUCCESS;

    while(fgets(line, LINE_LENGTH, as_file) != NULL){
        
        current_line = parser(line, line_number);

        if(current_line->line_directive != NULL){
            if(current_line->line_directive->directive_type == DIR_ENTRY){
                if(handel_entry(current_line) == ERROR){
                    error_flag = ERROR;
                }
            }
        }

        else if(current_line->line_type == LINE_COMMAND){
            if(update_label(current_line, ic, extern_list) == ERROR){
                error_flag = ERROR;
            }

            (*ic) += COMMAND_SIZE;
        }

        line_number++;
        free(current_line);
    }

    return error_flag;
}


static int handel_entry(AsLine* current_line)
{
    Symbol* current_label = search_label(current_line->target_label);

    if(current_label == NULL){
        printf("Error in line %d - label does not exist \n", current_line->line_number);
        return ERROR;
    }

    current_label->visibility = ENTRY;
    return SUCCESS;
}


static int update_label(AsLine* current_line, int* ic, ExternNode** extern_list)
{
    if(current_line->target_label[0] != '\0'){
        Symbol* label = search_label(current_line->target_label);

        if(label == NULL){
                printf("Error in line %d - label does not exist\n", current_line->line_number);
                return ERROR;
            }

        if(label->visibility == EXTERN){
            if(current_line->line_command->command_type == 'I'){
                printf("Error in line %d - target label in I command can't be extern\n", current_line->line_number);
                return ERROR;
            }

            if(update_extern(label, *ic, extern_list, current_line->line_number) == ERROR){
                return ERROR;
            }
        }
            update_label_binary(current_line, label->address, ic);
    }
    return SUCCESS;
}


static int update_extern(Symbol* label, int ic, ExternNode** extern_list, int line_number)
{
    ExternNode* new_extern = (ExternNode*) malloc(sizeof(ExternNode));
    ExternNode* temp = (*extern_list);

    if(new_extern == NULL){
        printf("Error in line %d - memory allocation failed\n", line_number);
        return ERROR;
    }

    strcpy(new_extern->label_name, label->label);
    new_extern->address = ic;
    new_extern->next = NULL;

    if((*extern_list) == NULL){
        (*extern_list) = new_extern;
        return SUCCESS;
    }

    while(temp->next != NULL){
        temp = temp->next;
    }

    temp->next = new_extern;
    return SUCCESS;
}


void free_extern_list(ExternNode* extern_list){

    while(extern_list != NULL){
        ExternNode* temp = extern_list->next;
        free(extern_list);
        extern_list = temp;
    }
}