#ifndef SCRIPT_H
#define SCRIPT_H

#include "../helper/helper.h"
#include "op.h"

#define CMDS_SIZE 520

typedef struct {
    uint8_t data[CMDS_SIZE];
    int32_t data_len;
} Command;

typedef struct {
    Command cmds[CMDS_SIZE];
    int32_t cmds_len;
} Script;

Script* Script_init();

void Script_free(Script* script);

void Script_toString(Script* script);

uint64_t Script_length(Script* script);

Script* p2pkh_script(uint8_t* h160);

void Script_set_cmds(Script* script, Command* cmds, int32_t cmds_len);

Script* Script_parse(uint8_t* s);

void Script_serialize(Script* script, uint8_t* result);

Script* Script_add(Script* script_1, Script* script_2);

bool Script_evaluate(Script* script, S256Field* z);

void cmds_deep_copy(Command* dest, Command* src, int32_t len);

void Script_deep_copy(Script* dest, Script* src);

bool is_p2pkh_script_pubkey(Script* script);

bool is_p2sh_script_pubkey(Script* script);

bool is_p2wpkh_script_pubkey(Script* script);

#endif // SCRIPT_H