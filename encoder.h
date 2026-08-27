/* encoder.h */
#ifndef ENCODER_H
#define ENCODER_H

#include "globals.h"
typedef struct as_line AsLine;


#define COMMANDS_QNT 27
#define R_ARITHMETIC_QNT 3
#define R_COPY_QNT 2
#define MIN_BRANCH_OPCODE 15
#define MAX_BRANCH_OPCODE 18

#define MIN_REG 0
#define MAX_REG 31
#define IMMED_MIN_VAL -32768
#define IMMED_MAX_VAL 32767

#define JMP_OPCODE 30
#define LA_OPCODE 31
#define CALL_OPCODE 32
#define HLT_OPCODE 63



typedef struct command {
    int opcode;
    int funct;
    char command_type;
    char command_name[COMMAND_LENGTH];
    int opperands_qnt;
} Command;

Command* search_command(char *search_str);
void convert_command(AsLine* current_line, int* ic);
void convert_directive(AsLine* current_line, int* dc);
void update_label_binary(AsLine* current_line, int label_address, int* ic);
const unsigned int* get_code_image();
const unsigned char* get_data_image();
#endif
