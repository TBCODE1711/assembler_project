/* parser.h */
#ifndef PARSER_H
#define PARSER_H

#include "globals.h"
#include "encoder.h"
#include "directives.h"

#define MAX_REG_QNT 3

typedef enum {
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_ERROR,
    LINE_COMMAND,
    LINE_DIRECTIVE,
    LINE_OK
} LineType;

typedef struct As_Line {
    Command* line_command;
    int registers[MAX_REG_QNT];
    int immed_num;
    char defined_lable[LINE_LENGTH];
    int directive_type;
    char str[LINE_LENGTH];
    int nums[LINE_LENGTH / 2];
    char target_label[LINE_LENGTH];
    int line_number;
    LineType line_type;
    struct As_Line* next_line;
} As_Line;


As_Line* parser(const char* line, int line_number, const char* file_name);

#endif