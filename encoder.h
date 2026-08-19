/* encoder.h */
#ifndef ENCODER_H
#define ENCODER_H
#define COMMAND_LENGTH 5
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

typedef struct Command {
    int opcode;
    int funct;
    char command_type;
    char command_name[COMMAND_LENGTH];
    int opperands_qnt;
} Command;


Command* search_command(char *search_str);
void convert_command(char command_type, )

#endif
