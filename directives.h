#ifndef DIRECTIVES_H
#define DIRECTIVES_H

#define DIRECTIVE_LENGTH 10
#define DB_LENGTH 8
#define DB_MIN -128
#define DB_MAX 127
#define DH_MIN -32768
#define DH_MAX 32767
#define DW_MIN -2147483648
#define DW_MAX 2147483647


typedef enum{
    DIR_DB,
    DIR_DH,
    DIR_DW,
    DIR_ASCIZ,
    DIR_ENTRY,
    DIR_EXTERN
} DirectiveType;

typedef struct {
    char directive_name[DIRECTIVE_LENGTH];
    int bytes_qnt;
    DirectiveType directive_type;
} Directive;

Directive* search_directive(char* search_name);

#endif
