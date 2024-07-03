#include "script.h"

Script* Script_init() {
    Script* script = malloc(sizeof(Script));
    for (int32_t i = 0; i < CMDS_SIZE; i++) {
        memset(script->cmds[i].data, 0, sizeof(script->cmds[i].data));
        script->cmds[i].data_len = 0;
    }
    script->cmds_len = 0;
    return script;
}

void Script_free(Script* script) {
    free(script);
}

void Script_toString(Script* script) {
    // for (int32_t i = 0; i < script->cmds_len; i++) {
    //     Command cmd = script->cmds[i];
    //     printf("Command: ");
    //     for (int32_t j = 0; j < cmd.data_len; j++) {
    //         printf("%02x", cmd.data[j]);
    //     }
    //     printf("\n");
    // }
    uint64_t length = Script_length(script);
    uint8_t result[length];
    Script_serialize(script, result);
    for (int32_t i = 0; i < length; i++) {
        printf("%02x", result[i]);
    }
    printf("\n");
}

uint64_t Script_length(Script* script) {
    uint64_t length = 0;
    for (int32_t i = 0; i < script->cmds_len; i++) {
        length += script->cmds[i].data_len;
        if (script->cmds[i].data_len > 1 && script->cmds[i].data_len <= 75) {
            length++;
        } else if (script->cmds[i].data_len >= 76 && script->cmds[i].data_len <= 255) {
            length += 2;
        } else if (script->cmds[i].data_len >= 256 && script->cmds[i].data_len <= 520) {
            length += 3;
        } else if (script->cmds[i].data_len > 520) {
            printf("script_length: Command length too long\n");
        }
    }
    length += add_varint_size(length);
    return length;
}

Script* p2pkh_script(uint8_t* h160) {
    Script* script = Script_init();
    Command cmds[5];
    cmds[0].data_len = 1;
    cmds[0].data[0] = 0x76;
    cmds[1].data_len = 1;
    cmds[1].data[0] = 0xa9;
    cmds[2].data_len = 20;
    memcpy(cmds[2].data, h160, 20);
    cmds[3].data_len = 1;
    cmds[3].data[0] = 0x88;
    cmds[4].data_len = 1;
    cmds[4].data[0] = 0xac;
    Script_set_cmds(script, cmds, 5);
    return script;
}

void Script_set_cmds(Script* script, Command* cmds, int32_t cmds_len) {
    for (int i = 0; i < cmds_len; i++) {
        memcpy(script->cmds[i].data, cmds[i].data, cmds[i].data_len);
        script->cmds[i].data_len = cmds[i].data_len;
    }
    script->cmds_len = cmds_len;
}

Script* Script_parse(uint8_t* s) {
    Script* script = Script_init();
    uint64_t length = read_varint(s);
    s += read_varint_size(s);
    int32_t count = 0;
    uint8_t current_byte = 0;
    while (count < length) {
        current_byte = s[count];
        count++;
        if (current_byte >= 1 && current_byte <= 75) {
            uint8_t n = current_byte;
            Command cmd;
            cmd.data_len = n;
            memcpy(cmd.data, s + count, n);
            script->cmds[script->cmds_len] = cmd;
            script->cmds_len++;
            count += n;
        } else if (current_byte == 76) {
            int32_t data_length = little_endian_to_int(s + count, 1);
            count++;
            Command cmd;
            cmd.data_len = data_length;
            memcpy(cmd.data, s + count, data_length);
            script->cmds[script->cmds_len] = cmd;
            script->cmds_len++;
            count += data_length;
        } else if (current_byte == 77) {
            int32_t data_length = little_endian_to_int(s + count, 2);
            count += 2;
            Command cmd;
            cmd.data_len = data_length;
            memcpy(cmd.data, s + count, data_length);
            script->cmds[script->cmds_len] = cmd;
            script->cmds_len++;
            count += data_length;
        } else {
            uint8_t op_code = current_byte;
            Command cmd;
            cmd.data_len = 1;
            cmd.data[0] = op_code;
            script->cmds[script->cmds_len] = cmd;
            script->cmds_len++;
        }
    }
    if (count != length) {
            printf("count: %d\n", count);
            printf("length: %lu\n", length);
            printf("count != length\n");
            printf("Parsing script failed\n");
            exit(1);
    }
    return script;
}

void Script_serialize(Script* script, uint8_t* result) {
    uint64_t length = 0;
    for (int32_t i = 0; i < script->cmds_len; i++) {
        length += script->cmds[i].data_len;
        if (script->cmds[i].data_len > 1) {
            length++;
        }
    }
    if (length >= 0xfd) {
        length++;
    }
    // Maybe this is right?
    // if (length < 0xfd) {

    // } else if (length <= 0xffff) {
    //     length++;
    // } else if (length <= 0xffffffff) {
    //     length += 3;
    // } else {
    //     length += 7;
    // }
    encode_varint(result, length);
    result += read_varint_size(result);
    for (int i = 0; i < script->cmds_len; i++) {
        if (script->cmds[i].data_len == 1) {
            
        } else if (script->cmds[i].data_len < 75) {
            int_to_little_endian(script->cmds[i].data_len, result, 1);
            result++;
        } else if (script->cmds[i].data_len < 0x100) {
            result[0] = 76;
            int_to_little_endian(script->cmds[i].data_len, result + 1, 1);
            result += 2;
        } else if (script->cmds[i].data_len < 520) {
            result[0] = 77;
            int_to_little_endian(script->cmds[i].data_len, result + 1, 2);
            result += 3;
        } else {
            printf("Command length too long\n");
            exit(1);
        }
        memcpy(result, script->cmds[i].data, script->cmds[i].data_len);
        result += script->cmds[i].data_len;
    }
}

Script* Script_add(Script* script_1, Script* script_2) {
    Script* script = Script_init();
    for (int32_t i = 0; i < script_1->cmds_len; i++) {
        script->cmds[i] = script_1->cmds[i];
        script->cmds_len++;
    }
    for (int32_t i = 0; i < script_2->cmds_len; i++) {
        script->cmds[script->cmds_len] = script_2->cmds[i];
        script->cmds_len++;
    }
    return script;
}

void cmds_deep_copy(Command* dest, Command* src, int32_t len) {
    for (int32_t i = 0; i < len; i++) {
        memcpy(dest[i].data, src[i].data, src[i].data_len);
        dest[i].data_len = src[i].data_len;
    }
}

void Script_deep_copy(Script* dest, Script* src) {
    cmds_deep_copy(dest->cmds, src->cmds, src->cmds_len);
    dest->cmds_len = src->cmds_len;
}

bool Script_evaluate(Script* script, S256Field* z) {
    Script* script_copy = Script_init();
    Script_deep_copy(script_copy, script);
    Op* op_1 = Op_init();
    Op* op_2 = Op_init();
    int32_t j = 0;
    //For p2sh
    int32_t original_cmds_len = script_copy->cmds_len;
    while (script_copy->cmds_len > 0) {
        Command cmd = script_copy->cmds[j];
        j++;
        script_copy->cmds_len--;
        // printf("Command: ");
        // for (int i = 0; i < cmd.data_len; i++) {
        //     printf("%02x", cmd.data[i]);
        // }
        // printf("\n");
        // printf("\n");
        if (cmd.data_len == 1) {
            int32_t cmd_int = little_endian_to_int(cmd.data, 1);
            // char* cmd_str = op_code_functions(cmd_int);
            // printf("%s\n", cmd_str);
            if (cmd_int == 172) {
                Op_perform_operation_z(op_1, cmd_int, z);
            } else if (cmd_int == 107 || cmd_int == 108) {
                Op_perform_operation_alt(op_1, cmd_int, op_2);
            } else {
                Op_perform_operation_basic(op_1, cmd_int);
            }
        } else {
            push(op_1, cmd.data, cmd.data_len);
            //Added code for p2sh
            if ((script_copy->cmds_len == 3) && (script_copy->cmds[0].data[0] == 0xa9) && (script_copy->cmds[1].data_len == 20) && (script_copy->cmds[2].data[0] == 0x87)) {
                script_copy->cmds_len -= 3;
                original_cmds_len -= 2;
                Command h160 = script_copy->cmds[original_cmds_len];
                original_cmds_len--;
                if (!Op_hash160(op_1)) {
                    return false;
                }
                push(op_1, h160.data, h160.data_len);
                if (!Op_equal(op_1)) {
                    return false;
                }
                if (!Op_verify(op_1)) {
                    printf("Bad p2sh h160\n");
                    return false;
                }
                int32_t redeem_script_len = 0;
                if (cmd.data_len > 0xfd) {
                    redeem_script_len++;
                }
                // Maybe this is right
                // if (cmd.data_len < 0xfd) {

                // } else if (cmd.data_len <= 0xffff) {
                //     redeem_script_len++;
                // } else if (cmd.data_len <= 0xffffffff) {
                //     redeem_script_len += 3;
                // } else {
                //     redeem_script_len += 7;
                // }
                redeem_script_len += cmd.data_len;
                uint8_t redeem_script[redeem_script_len];
                encode_varint(redeem_script, cmd.data_len);
                int32_t offset = read_varint_size(redeem_script);
                memcpy(redeem_script + offset, cmd.data, cmd.data_len);
                Script* redeem_script_parsed = Script_parse(redeem_script);
                script_copy->cmds_len += redeem_script_parsed->cmds_len;
                for (int32_t i = 0; i < redeem_script_parsed->cmds_len; i++) {
                    script_copy->cmds[original_cmds_len + i] = redeem_script_parsed->cmds[i];
                }
            }
        }
        // printf("Stack: ");
        // for (int i = 0; i < op_1->top + 1; i++) {
        //     for (int k = 0; k < op_1->element_length[i]; k++) {
        //         printf("%02x", op_1->stack[i][k]);
        //     }
        //     printf("\n");
        //     printf("\n");
        // }
        // printf("\n");
    }
    if (op_1->top == -1) {
        Op_free(op_1);
        Op_free(op_2);
        Script_free(script_copy);
        return false;
    }
    uint8_t result[1] = {0};
    pop(op_1, result);
    if (result[0] == 1) {
        Op_free(op_1);
        Op_free(op_2);
        Script_free(script_copy);
        return true;
    } else {
        Op_free(op_1);
        Op_free(op_2);
        Script_free(script_copy);
        printf("Script evaluation failed\n");
        return false;
    }
}

bool is_p2pkh_script_pubkey(Script* script) {
    if (script->cmds_len != 5) {
        return false;
    }
    if (script->cmds[0].data_len != 1) {
        return false;
    }
    if (script->cmds[0].data[0] != 0x76) {
        return false;
    }
    if (script->cmds[1].data_len != 1) {
        return false;
    }
    if (script->cmds[1].data[0] != 0xa9) {
        return false;
    }
    if (script->cmds[2].data_len != 20) {
        return false;
    }
    if (script->cmds[3].data_len != 1) {
        return false;
    }
    if (script->cmds[3].data[0] != 0x88) {
        return false;
    }
    if (script->cmds[4].data_len != 1) {
        return false;
    }
    if (script->cmds[4].data[0] != 0xac) {
        return false;
    }
    return true;
}

bool is_p2sh_script_pubkey(Script* script) {
    if (script->cmds_len != 3) {
        return false;
    }
    if (script->cmds[0].data_len != 1) {
        return false;
    }
    if (script->cmds[0].data[0] != 0xa9) {
        return false;
    }
    if (script->cmds[1].data_len != 20) {
        return false;
    }
    if (script->cmds[2].data_len != 1) {
        return false;
    }
    if (script->cmds[2].data[0] != 0x87) {
        return false;
    }
    return true;
}

bool is_p2wpkh_script_pubkey(Script* script) {
    if (script->cmds_len != 2) {
        return false;
    }
    if (script->cmds[0].data[0] != 0x00) {
        return false;
    }
    if (script->cmds[1].data_len != 20) {
        return false;
    }
    return true;
}