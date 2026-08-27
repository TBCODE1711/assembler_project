/* parser.h */
#ifndef PARSER_H
#define PARSER_H

#include "globals.h"
#include "directives.h"
#include "encoder.h"


typedef enum {
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_ERROR,
    LINE_COMMAND,
    LINE_DIRECTIVE,
    LINE_OK
} LineType;

struct as_line {
    Command* line_command;
    int registers[MAX_REG_QNT];
    int immed_num;
    char defined_lable[LINE_LENGTH];
    Directive* line_directive; 
    char str[LINE_LENGTH];
    int nums[LINE_LENGTH / 2];
    int nums_qnt;
    char target_label[LINE_LENGTH];
    int line_number;
    LineType line_type;
    struct as_line* next_line;
};

AsLine* parser(const char* line, int line_number);

#endif