#ifndef SYMBOLS_MANAGER_H
#include "globals.h"

#define SYMBOLS_MANAGER_H
#define DATA 2
#define CODE 1
#define DATA 2 
#define ENTRY 1
#define EXTERN -1
#define LOCAL 0

typedef struct Symbol {
    char label[LABEL_LENGTH];
    int address;
    int command_type;
    int visibility;
    struct Symbol* next_symbol;
} Symbol;

Symbol* search_label(Symbol* head, char* search_STR);
Symbol* add_label(Symbol* head, char* label, int address, int command_type, int visibility);
void update_data_address(Symbol* head, int icf);
void free_list(Symbol* head);

#endif