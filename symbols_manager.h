#ifndef SYMBOLS_MANAGER_H
#include "globals.h"

#define SYMBOLS_MANAGER_H
#define DATA 2
#define CODE 1
#define NONE 0

enum visibility {
    EXTERN = -1,
    LOCAL = 0,
    ENTRY = 1
};

typedef struct Symbol {
    char label[LABEL_LENGTH];
    int address;
    int command_type;
    int visibility;
    struct Symbol* next_symbol;
} Symbol;

Symbol* search_label(char* search_STR);
Symbol* add_label(char* label, int address, int command_type, int visibility);
Symbol* search_entry_label(Symbol* current_label);
void update_data_address(int icf);
void free_list();

#endif