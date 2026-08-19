#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

static LineType first_check(const char* line, int line_number);
static const char* check_line_label(const char* line, As_Line* current_line);
static void check_directive(const char* line, As_Line* current_line);
static void check_command(const char* line, As_Line* current_line);
static void d_parser(const char* line, As_Line* current_line, Directive* current_directive);
static void asciz_parser(const char* line, As_Line* current_line);
static void entry_extern_parser(const char* line, As_Line* current_line);
static void check_R(const char* line, As_Line* current_line);
static void check_I(const char* line, As_Line* current_line);
static void check_J(const char* line, As_Line* current_line);
static char* check_label(const char** line_ptr, char* label, int line_number);
static const char* check_comma(const char* line, int line_number);
static int check_reg(const char** line, int line_number);
static int check_end_line(const char* line, int line_number);


As_Line* parser(const char* line, int line_number, const char* file_name)
{   
    LineType current_line_type;
    const char* temp;
    As_Line* current_line = (As_Line*) malloc(sizeof(As_Line));

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


static const char* check_line_label(const char* line, As_Line* current_line)
{
    char temp_word[LINE_LENGTH];
    const char* temp_ptr = line;
    const char* dummy_ptr = temp_word;

    sscanf(temp_ptr, "%s", temp_word);

    if(temp_word[strlen(temp_word) - 1] != ':'){
        return line;
    }

    temp_word[strlen(temp_word) - 1] = '\0';

    if(check_label(&dummy_ptr, temp_word, current_line->line_number) == NULL){
        current_line->line_type = LINE_ERROR;
        return NULL;
    }
    strcpy(current_line->defined_lable, temp_word);

    temp_ptr = strchr(line, ':') + 1;
    return temp_ptr;

}


static void check_directive(const char* line, As_Line* current_line)
{
    char temp_word[LINE_LENGTH];
    Directive* current_directive;

    sscanf(line, "%s", temp_word);

    line += strlen(temp_word);

    current_directive = search_directive(temp_word);

    if(current_directive == NULL){
        printf("Error in line %d - iligal directive \n", current_line->line_number);
        current_line->line_type = LINE_ERROR;
    }   

    else{
        current_line->line_type = LINE_DIRECTIVE;

        while(isspace(*line)){
            line++;
        }

        switch(current_directive->directive_type){
            case DIR_DB:
            case DIR_DH:
            case DIR_DW:
                d_parser(line, current_line, current_directive);
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

static void check_command(const char* line, As_Line* current_line)
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




static void d_parser(const char* line, As_Line* current_line, Directive* current_directive)
{   char* temp_ptr;
    long num;
    int i = 0;

    if((*line) == ','){
        printf("Error in line %d - iligal comma \n", current_line->line_number);
        current_line->line_type = LINE_ERROR;
        return;
    }

    while((*line) != '\n' && (*line) != '\0'){
        num = strtol(line, (char**) &temp_ptr, 10);
            
        if(line == temp_ptr){
            printf("Error in line %d - iligal char in directive defenition \n", current_line->line_number);
            current_line->line_type = LINE_ERROR;
            return;
        }

        
        switch(current_directive->directive_type){
            case DIR_DB:
                    if(num < DB_MIN || num > DB_MAX){
                        printf("Error in line %d - number exceeded mx size of DB: %d\n", current_line->line_number, DB_LENGTH);
                        current_line->line_type = LINE_ERROR;
                        return;
                    }
            break;

            case DIR_DH:
                    if(num < DH_MIN || num > DH_MAX){
                        printf("Error in line %d - number exceeded max size of DH: %d\n", current_line->line_number, DB_LENGTH);
                        current_line->line_type = LINE_ERROR;
                        return;
                    }
            break;

            case DIR_DW:
                    if(num < DW_MIN || num > DW_MAX){
                        printf("Error in line %d - number exceeded max size of DW", current_line->line_number);
                        current_line->line_type = LINE_ERROR;
                        return;
                    }
            break;

            default:
                break;
        }

        current_line->nums[i] = num;
        i++;
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



static void asciz_parser(const char* line, As_Line* current_line)
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


static void entry_extern_parser(const char* line, As_Line* current_line)
{
    char temp_word[LABEL_LENGTH];

        while(isspace(*line)){
        line++;
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





static void check_R(const char* line, As_Line* current_line)
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


static void check_I(const char* line, As_Line* current_line)
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

        num = strtol(line, (char**) &temp_ptr, 10);
        
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


static void check_J(const char* line, As_Line* current_line)
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

    if((**line) == '$'){
        
        reg = strtol((*line) + 1, (char**) &temp_ptr, 10);

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





int main(void)
{
    int i;
    As_Line* result;
    
    /* Array of test descriptions */
    char *descriptions[] = {
        "1. Line length > 80 chars",
        "2. Extraneous text at the end",
        "3. Label length > 31 chars",
        "4. Label starts with non-alpha",
        "5. Label contains invalid chars",
        "6. Label is a reserved word",
        "7. Space before colon in label",
        "8. Missing comma between operands",
        "9. Leading comma before first operand",
        "10. Trailing comma after last operand",
        "11. Multiple consecutive commas",
        "12. Invalid directive name",
        "13a. .db out of bounds (> 127)",
        "13b. .db out of bounds (< -128)",
        "13c. .dh out of bounds (> 32767)",
        "13d. .dw out of bounds (overflow)",
        "14a. Invalid data type (float)",
        "14b. Invalid data type (string/chars)",
        "15a. .asciz missing closing quote",
        "15b. .asciz missing opening quote",
        "16a. .entry missing parameter",
        "16b. .entry too many parameters",
        "16c. .extern invalid label name",
        "17. Invalid command name",
        "18a. Wrong operand count (too few)",
        "18b. Wrong operand count (too many)",
        "19a. Register missing $",
        "19b. Register out of bounds (> 31)",
        "19c. Register out of bounds (< 0)",
        "19d. Register invalid chars after $",
        "20a. Immediate out of bounds (> 32767)",
        "20b. Immediate out of bounds (< -32768)",
        "20c. Immediate invalid type (chars)",
        "21a. Wrong operand type (label instead of reg)",
        "21b. Wrong operand type (reg instead of label in branch)"
    };

    /* Array of test inputs matching the descriptions */
    char *tests[] = {
        "MAIN: add $1, $2, $3 this line is intentionally made very very very very very very very long to exceed eighty characters",
        "add $1, $2, $3 garbage",
        "ThisLabelIsWayTooLongAndExceedsThirtyOneCharacters: add $1, $2, $3",
        "1MAIN: add $1, $2, $3",
        "MA@IN: add $1, $2, $3",
        "add: add $1, $2, $3",
        "MAIN : add $1, $2, $3",
        "add $1 $2, $3",
        "add ,$1, $2, $3",
        "add $1, $2, $3,",
        "add $1,,$2, $3",
        ".invalid 5, 6",
        ".db 300",
        ".db -200",
        ".dh 40000",
        ".dw 5000000000",
        ".db 5.5",
        ".db abc",
        ".asciz \"hello",
        ".asciz hello\"",
        ".entry",
        ".entry MAIN, LOOP",
        ".extern 1BADLABEL",
        "invalidcmd $1, $2, $3",
        "add $1, $2",
        "add $1, $2, $3, $4",
        "add 1, $2, $3",
        "add $35, $2, $3",
        "add $-1, $2, $3",
        "add $A, $2, $3",
        "addi $1, 50000, $2",
        "addi $1, -50000, $2",
        "addi $1, abc, $2",
        "add $1, MAIN, $3",
        "bne $1, $2, $3"
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);

    for (i = 0; i < num_tests; i++) {
        printf("--- Test %s ---\n", descriptions[i]);
        printf("Input: %s\n", tests[i]);
        
        result = parser(tests[i], i + 1, "test.as");
        
        if (result != NULL && result->line_type == LINE_ERROR) {
            printf("Result: PASSED (Error successfully caught)\n\n");
        } else {
            printf("Result: FAILED (Parser did not return LINE_ERROR)\n\n");
        }
        
        if (result != NULL) {
            free(result);
        }
    }

    return 0;
}