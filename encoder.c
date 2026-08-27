#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encoder.h"
#include "parser.h"
#include "directives.h"


#define R_UNUSED_BITS 6
#define FUNCT_BITS 5
#define REG_BITS 5
#define OPCODE_BITS 6


#define IMMED_BITS 16

#define ADDRESS_BITS 25
#define REG_FLAG_BITS 1


#define BYTE_MASK 0xFF
#define BYTE_2_MULTIPLIER 2
#define BYTE_3_MULTIPLIER 3


#define J_REG_ON 1


enum opcodes {
    R_ARITHMETIC_OP = 0,
    R_COPY_OP = 1,
    ADDI_OP = 10,
    SUBI_OP = 11,
    ANDI_OP = 12,
    ORI_OP = 13,
    NORI_OP = 14,
    BNE_OP = 15,
    BEQ_OP = 16,
    BLT_OP = 17,
    BGT_OP = 18,
    LB_OP = 19,
    SB_OP = 20,
    LW_OP = 21,
    SW_OP = 22,
    LH_OP = 23,
    SH_OP = 24,
    JMP_OP = 30,
    LA_OP = 31,
    CALL_OP = 32,
    HLT_OP = 63
};

/* Functs */
enum functs {
    NONE_FUNCT = 0,
    ADD_FUNCT = 1,
    SUB_FUNCT = 2,
    AND_FUNCT = 3,
    OR_FUNCT = 4,
    NOR_FUNCT = 5,
    MOVE_FUNCT = 1,
    MVHI_FUNCT = 2,
    MVLO_FUNCT = 3
};

/* Operand Counts */
enum operand_counts {
    NO_OPERANDS = 0,
    ONE_OPERAND = 1,
    TWO_OPERANDS = 2,
    THREE_OPERANDS = 3
};

static unsigned int code_image[CODE_IMAGE_LENGTH];
static unsigned char data_image[DATA_IMAGE_LENGTH];


static const Command commands_table[] = 
{
    {R_ARITHMETIC_OP, ADD_FUNCT,  'R', "add", THREE_OPERANDS},
    {R_ARITHMETIC_OP, SUB_FUNCT,  'R', "sub", THREE_OPERANDS},
    {R_ARITHMETIC_OP, AND_FUNCT,  'R', "and", THREE_OPERANDS},
    {R_ARITHMETIC_OP, OR_FUNCT,   'R', "or",  THREE_OPERANDS},
    {R_ARITHMETIC_OP, NOR_FUNCT,  'R', "nor", THREE_OPERANDS},
    {R_COPY_OP, MOVE_FUNCT, 'R', "move", TWO_OPERANDS},
    {R_COPY_OP, MVHI_FUNCT, 'R', "mvhi", TWO_OPERANDS},
    {R_COPY_OP, MVLO_FUNCT, 'R', "mvlo", TWO_OPERANDS},
    {ADDI_OP, NONE_FUNCT, 'I', "addi", THREE_OPERANDS},
    {SUBI_OP, NONE_FUNCT, 'I', "subi", THREE_OPERANDS},
    {ANDI_OP, NONE_FUNCT, 'I', "andi", THREE_OPERANDS},
    {ORI_OP, NONE_FUNCT, 'I', "ori",  THREE_OPERANDS},
    {NORI_OP, NONE_FUNCT, 'I', "nori", THREE_OPERANDS},
    {BNE_OP, NONE_FUNCT, 'I', "bne", THREE_OPERANDS},
    {BEQ_OP, NONE_FUNCT, 'I', "beq", THREE_OPERANDS},
    {BLT_OP, NONE_FUNCT, 'I', "blt", THREE_OPERANDS},
    {BGT_OP, NONE_FUNCT, 'I', "bgt", THREE_OPERANDS},
    {LB_OP, NONE_FUNCT, 'I', "lb", THREE_OPERANDS},
    {SB_OP, NONE_FUNCT, 'I', "sb", THREE_OPERANDS},
    {LW_OP, NONE_FUNCT, 'I', "lw", THREE_OPERANDS},
    {SW_OP, NONE_FUNCT, 'I', "sw", THREE_OPERANDS},
    {LH_OP, NONE_FUNCT, 'I', "lh", THREE_OPERANDS},
    {SH_OP, NONE_FUNCT, 'I', "sh", THREE_OPERANDS},
    {JMP_OP, NONE_FUNCT, 'J', "jmp", ONE_OPERAND},
    {LA_OP, NONE_FUNCT, 'J', "la", ONE_OPERAND},
    {CALL_OP, NONE_FUNCT, 'J', "call", ONE_OPERAND},
    {HLT_OP, NONE_FUNCT, 'J', "hlt", NO_OPERANDS}
};

typedef struct{
    unsigned int unused: R_UNUSED_BITS;
    unsigned int funct: FUNCT_BITS;
    unsigned int rd: REG_BITS;
    unsigned int rt: REG_BITS;
    unsigned int rs: REG_BITS;
    unsigned int opcode: OPCODE_BITS;
} R_Binary;

typedef struct{
    int immed: IMMED_BITS;
    unsigned rt: REG_BITS;
    unsigned rs: REG_BITS;
    unsigned int opcode: OPCODE_BITS;
} I_Binary;

typedef struct {
    unsigned int address : ADDRESS_BITS;
    unsigned int reg     : REG_FLAG_BITS;  
    unsigned int opcode  : OPCODE_BITS;  
} J_Binary;

typedef union{
    R_Binary r_command;
    I_Binary i_command;
    J_Binary j_command;
    unsigned int binary_command;
} BinaryComand;



Command* search_command(char* search_str)
{
    int i;

    for(i = 0; i < COMMANDS_QNT; i++){
        if(strcmp(commands_table[i].command_name, search_str) == 0){
            return (Command*)&(commands_table[i]);
        }
    }
    return NULL;
}



static void convert_R(AsLine* current_line, BinaryComand* new_command);
static void convert_I(AsLine* current_line, BinaryComand* new_command);
static void convert_J(AsLine* current_line, BinaryComand* new_command);
static void convert_db(AsLine* current_line, int* dc);
static void convert_dh(AsLine* current_line, int* dc);
static void convert_dw(AsLine* current_line, int* dc);
static void convert_asciz(AsLine* current_line, int* dc);



void convert_command(AsLine* current_line, int* ic)
{
    BinaryComand new_command;
    new_command.binary_command = 0;

    switch(current_line->line_command->command_type){
        case 'R':
            convert_R(current_line, &new_command);
            break;

        case 'I':
            convert_I(current_line, &new_command);
            break;

        case 'J':
            convert_J(current_line, &new_command);
            break;
    }

    code_image[((*ic) - INITIAL_IC) / COMMAND_SIZE] = new_command.binary_command;
    (*ic) += COMMAND_SIZE;
}


void convert_directive(AsLine* current_line, int* dc)
{
    switch(current_line->line_directive->directive_type){
        case DIR_DB:
            convert_db(current_line, dc);
            break;

        case DIR_DH:
            convert_dh(current_line, dc);
            break;

        case DIR_DW:
            convert_dw(current_line, dc);
            break;

        case DIR_ASCIZ:
            convert_asciz(current_line, dc);
            break;

        default:
            break;
    }
}


static void convert_R(AsLine* current_line, BinaryComand* new_command)
{

    new_command->r_command.funct = current_line->line_command->funct;
    new_command->r_command.opcode = current_line->line_command->opcode;
    new_command->r_command.unused = 0;

    if(current_line->line_command->opperands_qnt == R_ARITHMETIC_QNT){
        new_command->r_command.rs = current_line->registers[0];
        new_command->r_command.rt = current_line->registers[1];
        new_command->r_command.rd = current_line->registers[2];
    }

    else{
        new_command->r_command.rs = current_line->registers[0];
        new_command->r_command.rd = current_line->registers[1];
        new_command->r_command.rt = 0;
    }
}


static void convert_I(AsLine* current_line, BinaryComand* new_command)
{
    new_command->i_command.opcode = current_line->line_command->opcode;
    new_command->i_command.rs = current_line->registers[0];
    new_command->i_command.rt = current_line->registers[1];

    if(current_line->line_command->opcode >= MIN_BRANCH_OPCODE && current_line->line_command->opcode <= MAX_BRANCH_OPCODE){
        new_command->i_command.immed = 0;
    }

    else{
        new_command->i_command.immed = current_line->immed_num;
    }
}


static void convert_J(AsLine* current_line, BinaryComand* new_command)

{
    new_command->j_command.opcode = current_line->line_command->opcode;

    if(current_line->target_label[0] == '\0'){
        if(current_line->line_command->opcode != HLT_OPCODE){
            new_command->j_command.reg = J_REG_ON;
            new_command->j_command.address = current_line->registers[0];
        }
    }

}


static void convert_db(AsLine* current_line, int* dc)
{
    int i;

    for(i=0; i < current_line->nums_qnt; i++){
        data_image[*dc] = current_line->nums[i] & BYTE_MASK;
        (*dc)++;
    }
}

static void convert_dh(AsLine* current_line, int* dc)
{
    int i;

    for(i=0; i < current_line->nums_qnt; i++){
        data_image[*dc] = current_line->nums[i] & BYTE_MASK;
        (*dc)++;
        data_image[*dc] = (current_line->nums[i]>>CHAR_BITS) & BYTE_MASK;
        (*dc)++;
    }
}


static void convert_dw(AsLine* current_line, int* dc)
{
    int i;

    for(i=0; i < current_line->nums_qnt; i++){
        data_image[*dc] = current_line->nums[i] & BYTE_MASK;
        (*dc)++;

        data_image[*dc] = (current_line->nums[i]>>CHAR_BITS) & BYTE_MASK;
        (*dc)++;

        data_image[*dc] = (current_line->nums[i]>>CHAR_BITS * 2) & BYTE_MASK;
        (*dc)++;

        data_image[*dc] = (current_line->nums[i]>>CHAR_BITS * 3) & BYTE_MASK;
        (*dc)++;
    }
}


static void convert_asciz(AsLine* current_line, int* dc)
{
    char* ptr = current_line->str;

    while((*ptr) != '\0'){
        data_image[*dc] = (*ptr);
        ptr++;
        (*dc)++;
    }

    data_image[*dc] = '\0';
    (*dc)++;
}


void update_label_binary(AsLine* current_line, int label_address, int* ic)
{
    BinaryComand current_command;
    current_command.binary_command = code_image[((*ic) - INITIAL_IC) / COMMAND_SIZE];

    if(current_line->line_command->command_type == 'I'){
        current_command.i_command.immed = label_address - (*ic);
    }

    else if(current_line->line_command->command_type == 'J'){
        current_command.j_command.address = label_address;
    }

    code_image[((*ic) - INITIAL_IC) / COMMAND_SIZE] = current_command.binary_command;
}




const unsigned int* get_code_image()
{
    return code_image;
}

const unsigned char* get_data_image()
{
    return data_image;
}
