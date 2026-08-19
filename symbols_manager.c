#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbols_manager.h"


Symbol* search_label(Symbol* head, char* search_STR)
{
    while(head != NULL){
        if(strcmp(head->label, search_STR) == 0){
            return head;
        }

        head = head->next_symbol;
    }
    return NULL;
}

Symbol* add_label(Symbol* head, char* label, int address, int command_type, int visibility)
{   
    Symbol* temp = head; 

    Symbol* new_symbol = (Symbol*) malloc(sizeof(Symbol));
    strcpy(new_symbol->label, label);
    new_symbol->address = address;
    new_symbol->command_type = command_type;
    new_symbol->visibility = visibility;
    new_symbol->next_symbol = NULL;

    if(temp == NULL){
        return new_symbol;
    }

    while(temp->next_symbol != NULL){
        temp = temp->next_symbol;
    }

    temp->next_symbol = new_symbol;
    return head;

}

void update_data_address(Symbol* head, int icf)
{
    while(head != NULL){
        if(head->command_type == DATA){
            head->address += icf;
        }
        head = head->next_symbol;
    }
}

void free_list(Symbol* head)
{
    while(head != NULL){
        Symbol* temp = head->next_symbol;
        free(head);
        head = temp;
    }
}
