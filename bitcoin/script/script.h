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

void script_serialize(Script* script, unsigned char* result);

Script* script_add(Script* script_1, Script* script_2);

size_t script_evaluate(Script* script, S256Field* z);

void cmds_deep_copy(Command* dest, Command* src, int len);

void script_deep_copy(Script* dest, Script* src);

size_t script_evaluate(Script* script, S256Field* z);

#endif // SCRIPT_H