#include "script.h"

Script* script_init() {
    Script* script = malloc(sizeof(Script));
    for (int i = 0; i < CMDS_SIZE; i++) {
        memset(script->cmds[i].data, 0, sizeof(script->cmds[i].data));
        script->cmds[i].data_len = 0;
    }
    script->cmds_len = 0;
    return script;
}

void script_free(Script* script) {
    free(script);
}

void script_set_cmds(Script* script, Command* cmds, int cmds_len) {
    for (int i = 0; i < cmds_len; i++) {
        memcpy(script->cmds[i].data, cmds[i].data, cmds[i].data_len);
        script->cmds[i].data_len = cmds[i].data_len;
    }
    script->cmds_len = cmds_len;
}

Script* script_parse(unsigned char* s) {
    Script* script = script_init();
    unsigned long long length = read_varint(s);
    if (s[0] == 0xfd) {
        s += 3;
    } else if (s[0] == 0xfe) {
        s += 5;
    } else if (s[0] == 0xff) {
        s += 9;
    } else {
        s += 1;
    }
    int count = 0;
    while (count < length) {
        unsigned char current_byte = s[count];
        count++;
        if (current_byte >= 1 && current_byte <= 75) {
            unsigned char n = current_byte;
            Command cmd;
            cmd.data_len = n;
            memcpy(cmd.data, s + count, n);
            script->cmds[script->cmds_len] = cmd;
            script->cmds_len++;
            count += n;
        } else if (current_byte == 76) {
            int data_length = little_endian_to_int(s + count, 1);
            count++;
            Command cmd;
            cmd.data_len = data_length;
            memcpy(cmd.data, s + count, data_length);
            script->cmds[script->cmds_len] = cmd;
            script->cmds_len++;
            count += data_length;
        } else if (current_byte == 77) {
            int data_length = little_endian_to_int(s + count, 2);
            count += 2;
            Command cmd;
            cmd.data_len = data_length;
            memcpy(cmd.data, s + count, data_length);
            script->cmds[script->cmds_len] = cmd;
            script->cmds_len++;
            count += data_length;
        } else {
            unsigned char op_code = current_byte;
            Command cmd;
            cmd.data_len = 1;
            cmd.data[0] = op_code;
            script->cmds[script->cmds_len] = cmd;
            script->cmds_len++;
        }
    }
    if (count != length) {
            printf("count: %d\n", count);
            printf("length: %llu\n", length);
            printf("count != length\n");
            printf("Parsing script failed\n");
            exit(1);
    }
    return script;
}

void script_serialize(Script* script, unsigned char* result) {
    unsigned long long length = 0;
    for (int i = 0; i < script->cmds_len; i++) {
        length += script->cmds[i].data_len;
        if (script->cmds[i].data_len > 1) {
            length++;
        }
    }
    encode_varint(result, length);
    if (length < 0xfd) {
        result++;
    } else if (length <= 0xffff) {
        result[0] = 0xfd;
        result += 3;
    } else if (length <= 0xffffffff) {
        result[0] = 0xfe;
        result += 5;
    } else {
        result[0] = 0xff;
        result += 9;
    }
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

Script* script_add(Script* script_1, Script* script_2) {
    Script* script = script_init();
    for (int i = 0; i < script_1->cmds_len; i++) {
        script->cmds[i] = script_1->cmds[i];
        script->cmds_len++;
    }
    for (int i = 0; i < script_2->cmds_len; i++) {
        script->cmds[script->cmds_len] = script_2->cmds[i];
        script->cmds_len++;
    }
    return script;
}

void cmds_deep_copy(Command* dest, Command* src, int len) {
    for (int i = 0; i < len; i++) {
        memcpy(dest[i].data, src[i].data, src[i].data_len);
        dest[i].data_len = src[i].data_len;
    }
}

void script_deep_copy(Script* dest, Script* src) {
    cmds_deep_copy(dest->cmds, src->cmds, src->cmds_len);
    dest->cmds_len = src->cmds_len;
}

size_t script_evaluate(Script* script, S256Field* z) {
    Script* script_copy = script_init();
    script_deep_copy(script_copy, script);
    Op* op_1 = op_init();
    Op* op_2 = op_init();
    int j = 0;
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
            int cmd_int = little_endian_to_int(cmd.data, 1);
            // char* cmd_str = op_code_functions(cmd_int);
            // printf("%s\n", cmd_str);
            if (cmd_int == 172) {
                op_perform_operation_z(op_1, cmd_int, z);
            } else if (cmd_int == 107 || cmd_int == 108) {
                op_perform_operation_alt(op_1, cmd_int, op_2);
            } else {
                op_perform_operation_basic(op_1, cmd_int);
            }
        } else {
            push(op_1, cmd.data, cmd.data_len);
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
        op_free(op_1);
        op_free(op_2);
        script_free(script_copy);
        return 0;
    }
    unsigned char result[1] = {0};
    pop(op_1, result);
    if (result[0] == 1) {
        op_free(op_1);
        op_free(op_2);
        script_free(script_copy);
        return 1;
    } else {
        op_free(op_1);
        op_free(op_2);
        script_free(script_copy);
        printf("Script evaluation failed\n");
        return 0;
    }
}