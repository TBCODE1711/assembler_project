#ifndef OUTPUT_GENERATOR_H
#define OUTPUT_GENERATOR_H

#include "globals.h"
#include "second_pass.h"

int generate_output(char* file_name, const unsigned int* code_image, const unsigned char* data_image, int ic, int dc, ExternNode* externs_list);

#endif