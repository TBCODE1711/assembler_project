#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"
#include "pre_assembler.h"
#include "first_pass.h"
#include "second_pass.h"
#include "encoder.h"
#include "symbols_manager.h" 
#include "output_generator.h"


int main(int argc, char* argv[])
{
    int i;

    for(i = 1; i < argc; i++){

        FILE* current_file;
        FILE* am_file;
        char am_file_name[FILENAME_MAX];
        ExternNode* externs_list = NULL;
        int ic = INITIAL_IC;
        int dc = 0;

        current_file = fopen(argv[i], "r");
        if(current_file == NULL){
            printf("Error with open file %s\n", argv[i]);
            continue;
        }

        printf("Processing file: %s\n", argv[i]);

        strcpy(am_file_name, argv[i]);
        am_file_name[strlen(am_file_name) - AS_FILE_END_LENGTH] = '\0';
        strcat(am_file_name, ".am");

        am_file = fopen(am_file_name, "w+");
        if(am_file == NULL){
            printf("Error with open .am file for file %s\n", argv[i]);
            continue;
        }

        if(pre_assembler_process(current_file, am_file) == ERROR){
            fclose(current_file);
            fclose(am_file);
            continue;
        }

        rewind(am_file);
        if(first_pass(am_file, &ic, &dc) == ERROR){
            free_extern_list(externs_list);
            free_list();
            fclose(current_file);
            fclose(am_file);
            remove(am_file_name);
            continue;
        }

        ic = INITIAL_IC;
        rewind(am_file);

        if(second_pass(am_file, &ic, &externs_list) == ERROR){
            free_extern_list(externs_list);
            free_list();            
            fclose(current_file);
            fclose(am_file);
            remove(am_file_name);
            continue;
        }

        if(generate_output(argv[i], get_code_image(), get_data_image(), ic, dc, externs_list) == ERROR){
            free_extern_list(externs_list);
            free_list();
            fclose(current_file);
            fclose(am_file);
            continue;
        }

        free_extern_list(externs_list);
        free_list();
        fclose(current_file);
        fclose(am_file);
    }
    return 0;
}