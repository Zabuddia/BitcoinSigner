#ifndef SCRIPT_H
#define SCRIPT_H

#include "../helper/helper.h"
#include "op.h"

#define CMDS_SIZE 520

typedef struct {
    unsigned char data[CMDS_SIZE];
    int data_len;
} Command;

typedef struct {
    Command cmds[CMDS_SIZE];
    int cmds_len;
} Script;

Script* script_init();

void script_free(Script* script);

void script_set_cmds(Script* script, Command* cmds, int cmds_len);

Script* script_parse(unsigned char* s);

Script* script_add(Script* script_1, Script* script_2);

#endif // SCRIPT_H