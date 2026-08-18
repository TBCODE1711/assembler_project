#ifndef DIRECTIVES_H
#define DIRECTIVES_H

#define DIRECTIVE_LENGTH 10
#define DB_LENGTH 8

typedef enum{
    DIR_DB,
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

#endifss