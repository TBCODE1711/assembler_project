#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include <stdio.h>
#include "globals.h"

typedef struct extern_node{
    char label_name[LABEL_LENGTH];
    int address;
    struct extern_node* next;
} ExternNode;


int second_pass(FILE* as_file, int* ic, ExternNode** extern_list);
void free_extern_list(ExternNode* extern_list);

#endif