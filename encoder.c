#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encoder.h"



typedef struct Command{
    int opcode;
    int funct;
    char command_type;
    char command_name[5];
    int opperands_qnt;
} Command;


static const Command commands_table[] = 
{
    {0, 1, 'R', "add", 3},
    {0, 2, 'R', "sub", 3},
    {0, 3, 'R', "and", 3},
    {0, 4, 'R', "or", 3},
    {0, 5, 'R', "nor", 3},
    {1, 1, 'R', "move", 2},
    {1, 2, 'R', "mvhi", 2},
    {1, 3, 'R', "mvlo", 2},
    {10, 0, 'I', "addi", 3},
    {11, 0, 'I', "subi", 3},
    {12, 0, 'I', "andi", 3},
    {13, 0, 'I', "ori", 3},
    {14, 0, 'I', "nori", 3},
    {15, 0, 'I', "bne", 3},
    {16, 0, 'I', "beq", 3},
    {17, 0, 'I', "blt", 3},
    {18, 0, 'I', "bgt", 3},
    {19, 0, 'I', "lb", 3},
    {20, 0, 'I', "sb", 3},
    {21, 0, 'I', "lw", 3},
    {22, 0, 'I', "sw", 3},
    {23, 0, 'I', "lh", 3},
    {24, 0, 'I', "sh", 3},
    {30, 0, 'J', "jmp", 1},
    {31, 0, 'J', "la", 1},
    {32, 0, 'J', "call", 1},
    {63, 0, 'J', "hlt", 0}

};


Command* search_command(char search_str[5])
{
    int i;

    for(i = 0; i < COMMANDS_QNT; i++){
        if(strcmp(commands_table[i].command_name, search_str) == 0){
            return &(commands_table[i]);
        }
    }
    return NULL;
}