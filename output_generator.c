#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "output_generator.h"
#include "encoder.h"
#include "symbols_manager.h"
#include "second_pass.h"

static int generate_ob(char* file_name, const unsigned int* code_image, const unsigned char* data_image, int ic, int dc)
{   
    FILE* new_file;
    int i;
    int j;

    char new_file_name[FILENAME_MAX];
    strcpy(new_file_name, file_name);
    strcat(new_file_name, ".ob");

    new_file = fopen(new_file_name, "w");
    if(new_file == NULL){
            printf("Error with opening %s .ext file\n", file_name);
            return ERROR;
        }
    
    fprintf(new_file, "%d %d \n", ic - INITIAL_IC, dc);

    for(i = 0; i < (ic - INITIAL_IC) / COMMAND_SIZE; i++){
        const unsigned char* current_byte = (unsigned char*) &code_image[i];

        fprintf(new_file, "%04d ", i *(COMMAND_SIZE) + INITIAL_IC);

        for(j = 0; j < COMMAND_SIZE; j++){
            fprintf(new_file, "%02X ", (*current_byte));
            current_byte++;
        }
        fprintf(new_file, "\n");
    }

    for(i = 0; i < dc; i += BYTES_PER_ROW){
        fprintf(new_file, "%04d ", ic + i);

        for(j = 0; j < BYTES_PER_ROW && j < (dc - i); j++){
            fprintf(new_file, "%02X ", data_image[i + j]);
        }
        fprintf(new_file, "\n");
    }

    fclose(new_file);
    return SUCCESS;
}


static int generate_ent(char* file_name)
{
    Symbol* current_label = NULL;

    if(search_entry_label(current_label) != NULL){
        FILE* new_file;
        char new_file_name[FILENAME_MAX];

        strcpy(new_file_name, file_name);
        strcat(new_file_name, ".ent");

        new_file = fopen(new_file_name, "w");
        if(new_file == NULL){
            printf("Error with opening %s .ext file\n", file_name);
            return ERROR;
        }

        while((current_label = search_entry_label(current_label)) != NULL){
            fprintf(new_file, "%s %04d\n", current_label->label, current_label->address);
        }

        fclose(new_file);
    } 
    return SUCCESS;
}


static int generate_ext(char* file_name, ExternNode* externs_list)
{
    if(externs_list != NULL){
        FILE* new_file;
        char new_file_name[FILENAME_MAX];

        strcpy(new_file_name, file_name);
        strcat(new_file_name, ".ext");

        new_file = fopen(new_file_name, "w");
        if(new_file == NULL){
            printf("Error with opening %s .ext file\n", file_name);
            return ERROR;
        }

        while(externs_list != NULL){
            fprintf(new_file, "%s %04d\n", externs_list->label_name, externs_list->address);
            externs_list = externs_list->next;
        }

        fclose(new_file);
    }
    return SUCCESS;
}


int generate_output(char* file_name, const unsigned int* code_image, const unsigned char* data_image, int ic, int dc, ExternNode* externs_list)
{
    file_name[strlen(file_name) - AS_FILE_END_LENGTH] = '\0';

    if(generate_ob(file_name, code_image, data_image, ic, dc) == ERROR){
        return ERROR;
    }

    if(generate_ent(file_name) == ERROR){
        return ERROR;
    }

    if(generate_ext(file_name, externs_list) == ERROR){
        return ERROR;
    }

    return SUCCESS;
}


