#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbols_manager.h"

static Symbol* head = NULL;


Symbol* search_label(char* search_STR)
{
    Symbol* temp = head;

    while(temp != NULL){
        if(strcmp(temp->label, search_STR) == 0){
            return temp;
        }

        temp = temp->next_symbol;
    }
    return NULL;
}

Symbol* add_label(char* label, int address, int command_type, int visibility)
{   
    Symbol* temp = head; 

    Symbol* new_symbol = (Symbol*) malloc(sizeof(Symbol));
    strcpy(new_symbol->label, label);
    new_symbol->address = address;
    new_symbol->command_type = command_type;
    new_symbol->visibility = visibility;
    new_symbol->next_symbol = NULL;

    if(temp == NULL){
        head = new_symbol;
        return head;
    }

    while(temp->next_symbol != NULL){
        temp = temp->next_symbol;
    }

    temp->next_symbol = new_symbol;
    return head;

}

void update_data_address(int icf)
{
    Symbol* temp = head;
    while(temp != NULL){
        if(temp->command_type == DATA){
            temp->address += icf;
        }
        temp = temp->next_symbol;
    }
}


Symbol* search_entry_label(Symbol* current_label)
{
    if(head == NULL){
        return NULL;
    }
    if(current_label == NULL){
        current_label = head;
    }

    else{
        current_label = current_label->next_symbol;
    }

    while(current_label != NULL){
        if(current_label->visibility == ENTRY){
            return current_label;
        }
        current_label = current_label->next_symbol;
    }
    return NULL;
}

void free_list()
{
    while(head != NULL){
        Symbol* temp = head->next_symbol;
        free(head);
        head = temp;
    }
}
