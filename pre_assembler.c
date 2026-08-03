#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define LINE_LENGTH 82
#define ERROR -1
#define SUCCESS 1
#define MEMORY_ERROR 2

typedef struct Mcro_Line{
    char line[LINE_LENGTH];
    struct Mcro_Line* next_line;
    
} Mcro_Line;

typedef struct mcro{
    char macro_name[LINE_LENGTH];
    Mcro_Line* macro_content;
    struct mcro* next_macro;
} Mcro;

/* Function Prototypes */
int pre_assembler_process(FILE* source_file, FILE* dest_file, Mcro** marcos_list);
int add_macro(FILE* source_file, Mcro** marcos_list, char* line, int* line_count);
Mcro* search_macro(char* search_name, Mcro* head);
void replace_macro(FILE* dest_file, Mcro* macro);
void free_macro_list(Mcro* head);
int search_command(char* name);

int pre_assembler_process(FILE* source_file, FILE* dest_file, Mcro** marcos_list)
{
    char line[LINE_LENGTH];
    char first_word[LINE_LENGTH];
    Mcro* macro;
    int line_count = 0;

    while(fgets(line, LINE_LENGTH, source_file) != NULL){
        line_count++;
        if(sscanf(line, "%s", first_word) == 1){
            if (line[strlen(line) - 1] != '\n' && !feof(source_file)) {
            printf("Error - line %d exceeded %d chars", line_count, LINE_LENGTH);
            return ERROR;
            }   

        if(strcmp(first_word, "mcro") == 0){
            if(add_macro(source_file, marcos_list, line, &line_count) == ERROR){
                return ERROR;
            }
        }
        else if((macro = search_macro(first_word, *marcos_list)) != NULL){
            replace_macro(dest_file, macro);
        }

        else {
            fputs(line, dest_file);
        }
        }
    }
    return SUCCESS;
}

int add_macro(FILE* source_file, Mcro** marcos_list, char* line, int* line_count)
{
    char temp_name[LINE_LENGTH];
    char file_line[LINE_LENGTH];
    char first_word[LINE_LENGTH];
    char extra_word[LINE_LENGTH];
    int word_count;
    Mcro_Line* tail = NULL;
    Mcro* new_macro; 

    word_count = sscanf(line, "%*s %s %s" , temp_name, extra_word);

    if(word_count > 1){
        printf("Error - extra chars in macro defenition, line: %d", *line_count);
        return ERROR;
    }
    if(word_count < 1){
        printf("Error - missing macro name, line %d", *line_count);
        return ERROR;
    }

    if(search_command(temp_name) == SUCCESS){
        printf("Error - macro name can not be an command name, line %d", *line_count);
        return ERROR;
    }


    new_macro = (Mcro*) malloc(sizeof(Mcro));
    if (new_macro == NULL) {
    printf("Error: Memory allocation failed\n");
    return MEMORY_ERROR;
    }

    strcpy(new_macro->macro_name, temp_name);

    new_macro->macro_content = NULL;

    while (fgets(file_line, LINE_LENGTH, source_file) != NULL) {
         Mcro_Line* new_line;
        word_count = sscanf(file_line, "%s %s", first_word, extra_word);
        (*line_count)++;
        if(word_count > 0){
        if (file_line[strlen(file_line) - 1] != '\n' && !feof(source_file)) {
            printf("Error - line %d exceeded %d chars", *line_count, LINE_LENGTH);
            return ERROR;
        }
        if (strcmp(first_word, "mcroend") == 0) {
            if(word_count > 1){
            printf("Error - extra chars in macro end, line: %d", *line_count);
            return ERROR;
        }
            break; 
        }

        new_line = (Mcro_Line*) malloc(sizeof(Mcro_Line));
        if (new_line == NULL) {
            printf("Error: Memory allocation failed\n");
            return MEMORY_ERROR;
            }
        strcpy(new_line->line, file_line);
        new_line->next_line = NULL;

        if(new_macro->macro_content == NULL){
            new_macro->macro_content = new_line;
        }

        else{
            tail->next_line = new_line;
        }

        tail = new_line;
    }
}
    new_macro->next_macro = *marcos_list;
    *marcos_list = new_macro;
    return SUCCESS;
}


Mcro* search_macro(char* search_name, Mcro* head)
{
    while(head != NULL){
        if(strcmp(head->macro_name, search_name) == 0){
            return head;
        }

        head = head->next_macro;
    }
    return NULL;
}

void replace_macro(FILE* dest_file, Mcro* macro)
{
    Mcro_Line* current_line = macro->macro_content;

    while(current_line != NULL){
        fputs(current_line->line, dest_file);

        current_line = current_line->next_line;
    }
}

void free_macro_list(Mcro* head)
{
    Mcro* temp;

    while(head != NULL){
        Mcro_Line* local_line = head->macro_content;

        while(local_line != NULL){
            Mcro_Line* temp_line = local_line->next_line;
            free(local_line);
            local_line = temp_line;
            
        }
        
        temp = head->next_macro;
        free(head);
        head = temp;
    }
}

