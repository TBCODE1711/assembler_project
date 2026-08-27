#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "parser.h"
#include "encoder.h"
#include "directives.h"

#define DECIMAL_BASE 10

static LineType first_check(const char* line, int line_number);
static const char* check_line_label(const char* line, AsLine* current_line);
static void check_directive(const char* line, AsLine* current_line);
static void check_command(const char* line, AsLine* current_line);
static void d_parser(const char* line, AsLine* current_line);
static void asciz_parser(const char* line, AsLine* current_line);
static void entry_extern_parser(const char* line, AsLine* current_line);
static void check_R(const char* line, AsLine* current_line);
static void check_I(const char* line, AsLine* current_line);
static void check_J(const char* line, AsLine* current_line);
static char* check_label(const char** line_ptr, char* label, int line_number);
static const char* check_comma(const char* line, int line_number);
static int check_reg(const char** line, int line_number);
static int check_end_line(const char* line, int line_number);
static int check_missing_ops(const char* line, int line_number);


AsLine* parser(const char* line, int line_number)
{   
    LineType current_line_type;
    const char* temp;
    AsLine* current_line = (AsLine*) calloc(1, sizeof(AsLine));

    if(current_line == NULL){
        printf("Error in  line %d - memory allocation failed \n", line_number);
        return NULL;
    }

     current_line->line_number = line_number;

    if((current_line_type = first_check(line, line_number)) != LINE_OK){
        current_line->line_type = current_line_type;
        return current_line;
    }

    temp = check_line_label(line, current_line);

    
    if(temp == NULL){
        current_line->line_type = LINE_ERROR;
        return current_line;
    }

    while(isspace(*temp)){
        temp++;
    }

    if((*temp) == '.'){
        check_directive(temp, current_line);
    }

    else{
        check_command(temp, current_line);
    }

    return current_line;
}


static LineType first_check(const char* line, int line_number)
{   
    if(strlen(line) > LINE_LENGTH){
        printf("Error in line %d - line length exceeded %d \n", line_number, LINE_LENGTH);
        return LINE_ERROR;
    }

    while(isspace(*line)){
        line++;
    }

    if((*line) == '\n' || (*line) == '\0'){
        return LINE_EMPTY;
    }
     else if((*line) == ';'){
        return LINE_COMMENT;
    }

    return LINE_OK;
}


static const char* check_line_label(const char* line, AsLine* current_line)
{
    char temp_word[LINE_LENGTH];
    const char* temp_ptr = line;
    const char* dummy_ptr = temp_word;

    sscanf(temp_ptr, "%s", temp_word);

    if(temp_word[strlen(temp_word) - 1] != ':'){
        return line;
    }

    temp_word[strlen(temp_word) - 1] = '\0';

    if(check_missing_ops(line, current_line->line_number) == ERROR){
        return NULL;
    }

    if(check_label(&dummy_ptr, temp_word, current_line->line_number) == NULL){
        current_line->line_type = LINE_ERROR;
        return NULL;
    }
    strcpy(current_line->defined_lable, temp_word);

    temp_ptr = strchr(line, ':') + 1;
    return temp_ptr;

}


static void check_directive(const char* line, AsLine* current_line)
{
    char temp_word[LINE_LENGTH];

    sscanf(line, "%s", temp_word);

    line += strlen(temp_word);

    current_line->line_directive = search_directive(temp_word);

    if(current_line->line_directive == NULL){
        printf("Error in line %d - iligal directive \n", current_line->line_number);
        current_line->line_type = LINE_ERROR;
    }   

    else{
        current_line->line_type = LINE_DIRECTIVE;

        while(isspace(*line)){
            line++;
        }

        switch(current_line->line_directive->directive_type){
            case DIR_DB:
            case DIR_DH:
            case DIR_DW:
                d_parser(line, current_line);
                break;

            case DIR_ASCIZ:
                asciz_parser(line, current_line);
                break;

            case DIR_ENTRY:
            case DIR_EXTERN:
                entry_extern_parser(line, current_line);
                break;

            default:
                break;
        }
    }
}

static void check_command(const char* line, AsLine* current_line)
{
    char temp_word[LINE_LENGTH];
    Command* current_command;

    sscanf(line, "%s", temp_word);

    line += strlen(temp_word);

    if((current_command = search_command(temp_word)) == NULL){
        printf("Error in line %d - command does not exist \n", current_line->line_number);
        current_line->line_type = LINE_ERROR;
    }

    else{
        current_line->line_type = LINE_COMMAND;
        current_line->line_command = current_command;

        while(isspace(*line)){
            line++;
        }

       switch(current_command->command_type){
        case 'R':
            check_R(line, current_line);
            break;
        
        case 'I':
            check_I(line, current_line);
            break;

        case 'J':
            check_J(line, current_line);
            break;

        default:
            break;

       } 
    }
}   




static void d_parser(const char* line, AsLine* current_line)
{   char* temp_ptr;
    long num;
    int i = 0;

    if((*line) == ','){
        printf("Error in line %d - iligal comma \n", current_line->line_number);
        current_line->line_type = LINE_ERROR;
        return;
    }

    while((*line) != '\n' && (*line) != '\0'){
        num = strtol(line, (char**) &temp_ptr, DECIMAL_BASE);
            
        if(line == temp_ptr){
            printf("Error in line %d - iligal char in directive defenition \n", current_line->line_number);
            current_line->line_type = LINE_ERROR;
            return;
        }

        
        switch(current_line->line_directive->directive_type){
            case DIR_DB:
                    if(num < DB_MIN || num > DB_MAX){
                        printf("Error in line %d - number exceeded mx size of DB: %d - %d\n", current_line->line_number, DB_MAX, DB_MIN);
                        current_line->line_type = LINE_ERROR;
                        return;
                    }
            break;

            case DIR_DH:
                    if(num < DH_MIN || num > DH_MAX){
                        printf("Error in line %d - number exceeded max size of DH: %d - %d\n", current_line->line_number, DH_MAX, DH_MIN);
                        current_line->line_type = LINE_ERROR;
                        return;
                    }
            break;

            case DIR_DW:
                    if(num < DW_MIN || num > DW_MAX){
                        printf("Error in line %d - number exceeded max size of DW: %d - %ld", current_line->line_number, DW_MAX, DW_MIN);
                        current_line->line_type = LINE_ERROR;
                        return;
                    }
            break;

            default:
                break;
        }

        current_line->nums[i] = num;
        i++;
        current_line->nums_qnt++;
        line = temp_ptr;

        while(isspace(*line)){
            line++;
        }

        if(((*line) != '\0' && (*line) != '\n')){
            if((line = check_comma(line, current_line->line_number)) == NULL){
                current_line->line_type = LINE_ERROR;
                return;
            }

        if((*line) == '\n' || (*line) == '\0'){
            printf("Error in line %d - iligal comma \n", current_line->line_number);
            current_line->line_type = LINE_ERROR;
            return;
        }
    }
}
}



static void asciz_parser(const char* line, AsLine* current_line)
{
    int i = 0;

    while(isspace(*line) != 0){
        line++;
    }

    if((*line) != '\"'){
        printf("Error in line %d - missing \" \n", current_line->line_number);
        current_line->line_type = LINE_ERROR;
        return;
    }

    line++;

    while(isprint(*line) != 0 && (*line) != '\"'){
        current_line->str[i] = (*line);
        i++;
        line++;
    }
    current_line->str[i] = '\0';

    if((*line) == '\"'){
        if(check_end_line(line + 1, current_line->line_number) == ERROR){
            return;
        }
        return;
    }

    if((*line) == '\0' || (*line) == '\n'){
        printf("Error in line %d - missing \" \n", current_line->line_number);
        current_line->line_type = LINE_ERROR;
        return;
    }

    printf("Error in line %d - iligal char in the string", current_line->line_number);
    current_line->line_type = LINE_ERROR;
}


static void entry_extern_parser(const char* line, AsLine* current_line)
{
    char temp_word[LABEL_LENGTH];

        while(isspace(*line)){
        line++;
    }

    if(check_missing_ops(line, current_line->line_number) == ERROR){
        current_line->line_type = LINE_ERROR;
        return;
    }

    if(check_label(&line, temp_word, current_line->line_number) != NULL){
        strcpy(current_line->target_label, temp_word);
    }

    else{
        current_line->line_type = LINE_ERROR;
        return;
    }

    if(check_end_line(line, current_line->line_number) == ERROR){   
        current_line->line_type = LINE_ERROR;
    }
}





static void check_R(const char* line, AsLine* current_line)
{
    int temp_reg;
    int i;

    for(i = 0 ; i < current_line->line_command->opperands_qnt; i++){

        if((temp_reg = check_reg(&line, current_line->line_number)) != ERROR){
            current_line->registers[i] = temp_reg;

            if(i < (current_line->line_command->opperands_qnt - 1)){
                if((line = check_comma(line, current_line->line_number)) == NULL){
                    current_line->line_type = LINE_ERROR;
                    return;
                }
            }
        }

        else{
            current_line->line_type = LINE_ERROR;
            return;
        }
    }

    if(check_end_line(line, current_line->line_number) == ERROR){
        current_line->line_type = LINE_ERROR;
        return;
    }
}


static void check_I(const char* line, AsLine* current_line)
{
    char temp_word[LABEL_LENGTH];
    char* temp_ptr;
    int num;

    if((current_line->line_command->opcode) >= MIN_BRANCH_OPCODE && (current_line->line_command->opcode) <= MAX_BRANCH_OPCODE){
        
        if((current_line->registers[0] = check_reg(&line, current_line->line_number)) == ERROR){
            current_line->line_type = LINE_ERROR;
            return;
        }

        if((line = check_comma(line, current_line->line_number)) == NULL){
            current_line->line_type = LINE_ERROR;
            return;
        }

        if((current_line->registers[1] = check_reg(&line, current_line->line_number)) == ERROR){
            current_line->line_type = LINE_ERROR;
            return;
        }

        if((line = check_comma(line, current_line->line_number)) == NULL){
            current_line->line_type = LINE_ERROR;
            return;
        }

        if(check_missing_ops(line, current_line->line_number) == ERROR){
            current_line->line_type = LINE_ERROR;
            return;
        }

        if(check_label(&line, temp_word, current_line->line_number) == NULL){
            current_line->line_type = LINE_ERROR;
            return;
        }

        strcpy(current_line->target_label, temp_word);
        
        while(isspace(*line)){
            line++;
        }

        if(check_end_line(line, current_line->line_number) == ERROR){
            current_line->line_type = LINE_ERROR;
            return;
        }
    }

    else{
        if((current_line->registers[0] = check_reg(&line, current_line->line_number)) == ERROR){
            current_line->line_type = LINE_ERROR;
            return;
        }

        if((line = check_comma(line, current_line->line_number)) == NULL){
            current_line->line_type = LINE_ERROR;
            return;
        }

        num = strtol(line, (char**) &temp_ptr, DECIMAL_BASE);
        
        if(line == temp_ptr){
            printf("Error in line %d - iligal immed number \n", current_line->line_number);
            current_line->line_type = LINE_ERROR;
            return;
        }

        line = temp_ptr;

        if(num < IMMED_MIN_VAL || num > IMMED_MAX_VAL){
            printf("Error in line %d - immed number exceeded limit of 16 bits \n", current_line->line_number);
            current_line->line_type = LINE_ERROR;
            return;
        }

        current_line->immed_num = num;

        if((line = check_comma(line, current_line->line_number)) == NULL){
            current_line->line_type = LINE_ERROR;
            return;
        }

        if((current_line->registers[1] = check_reg(&line, current_line->line_number)) == ERROR){
            current_line->line_type = LINE_ERROR;
            return;
        }

        if(check_end_line(line, current_line->line_number) == ERROR){
            current_line->line_type = LINE_ERROR;
        } 
    }
}


static void check_J(const char* line, AsLine* current_line)
{   
    char temp_word[LABEL_LENGTH];

    switch(current_line->line_command->opcode){
        case JMP_OPCODE:
            if((*line) == '$'){
                if((current_line->registers[0] = check_reg(&line, current_line->line_number)) == ERROR){
                    current_line->line_type = LINE_ERROR;
                    return;
                }
            }
            else{
                if(check_missing_ops(line, current_line->line_number) == ERROR){
                    current_line->line_type = LINE_ERROR;
                    return;
                }

                if(check_label(&line, temp_word, current_line->line_number) == NULL){
                    current_line->line_type = LINE_ERROR;
                    return;
                }

                strcpy(current_line->target_label, temp_word);

                if(check_end_line(line, current_line->line_number) == ERROR){
                    current_line->line_type = LINE_ERROR;
                    return;
                }
            }
            break;

            default:
                break;

        case LA_OPCODE:
        case CALL_OPCODE:

            if(check_missing_ops(line, current_line->line_number) == ERROR){
            current_line->line_type = LINE_ERROR;
            return;
            }

            if(check_label(&line, temp_word, current_line->line_number) == NULL){
                current_line->line_type = LINE_ERROR;
                return;
            }

            if(check_end_line(line, current_line->line_number) == ERROR){
                current_line->line_type = LINE_ERROR;
                return;
            }

            strcpy(current_line->target_label, temp_word);
            break;
        

        case HLT_OPCODE:
            if(check_end_line(line, current_line->line_number) == ERROR){
                current_line->line_type = LINE_ERROR;
                return;
            }
            break;
    }
}




static char* check_label(const char** line_ptr, char* label, int line_number){
    int i;
    char temp_label[LINE_LENGTH];

    while(isspace(**line_ptr)){
        (*line_ptr)++;
    }

    sscanf((*line_ptr), "%s", temp_label);


    if(strlen(temp_label) > LABEL_LENGTH ){
        printf("Error in line %d - label name exceeded %d chars \n", line_number, LABEL_LENGTH);
        return NULL;
    }

    if(isalpha(temp_label[0]) == 0){
        printf("Error in line %d - label name must start with letter \n", line_number);
        return NULL;
    }

    for(i = 1; i < strlen(temp_label); i++){
        if(isalnum(temp_label[i]) == 0){
            printf("Error in line %d - label can be only letters and numbers \n", line_number);
            return NULL;
        }
    }

    if(search_command(temp_label) != NULL || search_directive(temp_label) != NULL){
        printf("Error in line %d - label name can not be command name \n", line_number);
        return NULL;
    }

    (*line_ptr) += strlen(temp_label);

    strcpy(label, temp_label);
    return label;
}


static const char* check_comma(const char* line, int line_number)
{
    while(isspace(*line)){
        line++;
    }

    if(check_missing_ops(line, line_number) == ERROR){
        return NULL;
    }

    if((*line) != ','){
        printf("Error in line %d - missing comma \n", line_number);
        return NULL;
    }

    line++;

    while(isspace(*line)){
        line++;
    }

    return line;
}


static int check_reg(const char** line, int line_number)
{
    int reg;
    const char* temp_ptr = (*line);

    while(isspace(**line)){
        (*line)++;
    }

    if(check_missing_ops(*line, line_number) == ERROR){
        return ERROR;
    }

    if((**line) == '$'){
        
        reg = strtol((*line) + 1, (char**) &temp_ptr, DECIMAL_BASE);

        if((*line) + 1!= temp_ptr){
            if(reg >= MIN_REG && reg <= MAX_REG){

                (*line) = temp_ptr;
                return reg;
            }
        }

        printf("Error in line %d - register number must be a number between %d to %d \n", line_number, MIN_REG, MAX_REG);
        return ERROR;
    }

    printf("Error in line %d - missing $ \n", line_number);
    return ERROR;
}

static int check_end_line(const char* line, int line_number)
{
    while(isspace(*line)){
        line++;
    }

    if((*line) == '\0' || (*line) == '\n'){
        return SUCCESS;
    }

    printf("Error in line %d - extra chars at the end of the line\n", line_number);
    return ERROR;
}


static int check_missing_ops(const char* line, int line_number) 
{
    while(isspace(*line)){
        line++;
    }

    if ((*line) == '\0' || (*line) == '\n') {
        printf("Error in line %d - missing operand\n", line_number);
        return ERROR;
    }
    return SUCCESS;
}

