#include "op.h"

Op* op_init() {
    Op* op = malloc(sizeof(Op));
    op->top = -1;
    for (int i = 0; i < STACK_SIZE; i++) {
        memset(op->stack[i], 0, sizeof(op->stack[i]));
        op->element_length[i] = 0;
    }
    return op;
}

void op_free(Op* op) {
    free(op);
}

void push(Op* op, unsigned char* element, size_t element_len) {
    if (op->top == STACK_SIZE - 1) {
        printf("Op stack overflow\n");
        exit(1);
    }
    op->top++;
    memcpy(op->stack[op->top], element, element_len);
    op->element_length[op->top] = element_len;
}

void pop(Op* op, unsigned char* result) {
    if (op->top == -1) {
        printf("Op stack underflow\n");
        exit(1);
    }
    size_t element_len = op->element_length[op->top];
    memcpy(result, op->stack[op->top], element_len);
    op->top--;
}

void peek(Op* op, unsigned char* result) {
    if (op->top == -1) {
        printf("Op stack underflow\n");
        exit(1);
    }
    size_t element_len = op->element_length[op->top];
    printf("\n");
    memcpy(result, op->stack[op->top], element_len);
}

void encode_num(long long num, unsigned char* result) {
    if (num == 0) {
        result[0] = 0;
        return;
    }
    long long abs_num = llabs(num);
    size_t negative = num < 0 ? 1 : 0;
    size_t result_len = 0;
    while (abs_num > 0) {
        result[result_len++] = abs_num & 0xff;
        abs_num >>= 8;
    }
    if (result[result_len - 1] & 0x80) {
        result[result_len++] = negative ? 0x80 : 0;
    } else if (negative) {
        result[result_len - 1] |= 0x80;
    }
}

long long decode_num(unsigned char* element, size_t element_len) {
    if (element[0] == 0) {
        return 0;
    }
    long long result = 0;
    little_endian_to_big_endian(element, element_len);
    size_t negative;
    if (element[0] & 0x80) {
        negative = 1;
        result = element[0] & 0x7f;
    } else {
        negative = 0;
        result = element[0];
    }
    for (int i = 1; i < element_len; i++) {
        result <<= 8;
        result += element[i];
    }
    if (negative) {
        result = -result;
    }
    little_endian_to_big_endian(element, element_len);
    return result;
}

size_t op_0(Op* op) {
    unsigned char element[1];
    encode_num(0, element);
    push(op, element, 1);
    return 1;
}

size_t op_1negate(Op* op) {
    unsigned char element[1];
    encode_num(-1, element);
    push(op, element, 1);
    return 1;
}

size_t op_1(Op* op) {
    unsigned char element[1];
    encode_num(1, element);
    push(op, element, 1);
    return 1;
}

size_t op_2(Op* op) {
    unsigned char element[1];
    encode_num(2, element);
    push(op, element, 1);
    return 1;
}

size_t op_3(Op* op) {
    unsigned char element[1];
    encode_num(3, element);
    push(op, element, 1);
    return 1;
}

size_t op_4(Op* op) {
    unsigned char element[1];
    encode_num(4, element);
    push(op, element, 1);
    return 1;
}

size_t op_5(Op* op) {
    unsigned char element[1];
    encode_num(5, element);
    push(op, element, 1);
    return 1;
}

size_t op_6(Op* op) {
    unsigned char element[1];
    encode_num(6, element);
    push(op, element, 1);
    return 1;
}

size_t op_7(Op* op) {
    unsigned char element[1];
    encode_num(7, element);
    push(op, element, 1);
    return 1;
}

size_t op_8(Op* op) {
    unsigned char element[1];
    encode_num(8, element);
    push(op, element, 1);
    return 1;
}

size_t op_9(Op* op) {
    unsigned char element[1];
    encode_num(9, element);
    push(op, element, 1);
    return 1;
}

size_t op_10(Op* op) {
    unsigned char element[1];
    encode_num(10, element);
    push(op, element, 1);
    return 1;
}

size_t op_11(Op* op) {
    unsigned char element[1];
    encode_num(11, element);
    push(op, element, 1);
    return 1;
}

size_t op_12(Op* op) {
    unsigned char element[1];
    encode_num(12, element);
    push(op, element, 1);
    return 1;
}

size_t op_13(Op* op) {
    unsigned char element[1];
    encode_num(13, element);
    push(op, element, 1);
    return 1;
}

size_t op_14(Op* op) {
    unsigned char element[1];
    encode_num(14, element);
    push(op, element, 1);
    return 1;
}

size_t op_15(Op* op) {
    unsigned char element[1];
    encode_num(15, element);
    push(op, element, 1);
    return 1;
}

size_t op_16(Op* op) {
    unsigned char element[1];
    encode_num(16, element);
    push(op, element, 1);
    return 1;
}

size_t op_nop(Op* op) {
    return 1;
}

size_t op_verify(Op* op) {
    unsigned char element[op->element_length[op->top]];
    pop(op, element);
    long long num = decode_num(element, op->element_length[op->top]);
    if (num == 0) {
        return 0;
    }
    return 1;
}

size_t op_return(Op* op) {
    return 0;
}

size_t op_depth(Op* op) {
    unsigned char element[1];
    encode_num(op->top + 1, element);
    push(op, element, 1);
    return 1;
}

size_t op_drop(Op* op) {
    unsigned char element[op->element_length[op->top]];
    pop(op, element);
    return 1;
}

size_t op_dup(Op* op) {
    if (op->top == -1) {
        return 0;
    }
    unsigned char element[op->element_length[op->top]];
    memcpy(element, op->stack[op->top], op->element_length[op->top]);
    push(op, element, op->element_length[op->top]);
    return 1;
}

size_t op_hash160(Op* op) {
    unsigned char element[op->element_length[op->top]];
    size_t element_len = op->element_length[op->top];
    pop(op, element);
    unsigned char hash[20];
    hash160(element, element_len, hash);
    push(op, hash, 20);
    return 1;
}

size_t op_ripemd160(Op* op) {
    unsigned char element[op->element_length[op->top]];
    size_t element_len = op->element_length[op->top];
    pop(op, element);
    unsigned char hash[20];
    ripemd160(element, element_len, hash);
    push(op, hash, 20);
    return 1;
}

size_t op_sha256(Op* op) {
    unsigned char element[op->element_length[op->top]];
    size_t element_len = op->element_length[op->top];
    pop(op, element);
    unsigned char hash[32];
    sha256(element, element_len, hash);
    push(op, hash, 32);
    return 1;
}

size_t op_hash256(Op* op) {
    unsigned char element[op->element_length[op->top]];
    size_t element_len = op->element_length[op->top];
    pop(op, element);
    unsigned char hash[32];
    hash256(element, element_len, hash);
    push(op, hash, 32);
    return 1;
}

size_t op_checksig(Op* op, S256Field* z) {
    if (op->top < 1) {
        return 0;
    }
    unsigned char sec_pubkey[op->element_length[op->top]];
    pop(op, sec_pubkey);
    unsigned char der_signature[op->element_length[op->top]];
    pop(op, der_signature);
    S256Point* point = S256Point_parse_sec(sec_pubkey);
    Signature* sig = Signature_parse(der_signature);
    int result = S256Point_verify(point, z, sig);
    S256Point_free(point);
    Signature_free(sig);
    if (result) {
        unsigned char element[1];
        encode_num(1, element);
        push(op, element, 1);
    } else {
        unsigned char element[1];
        encode_num(0, element);
        push(op, element, 1);
    }
    return 1;
}

char* op_code_functions(int cmd_int) {
    switch (cmd_int) {
        case 0:
            return "OP_0";
        case 79:
            return "OP_1NEGATE";
        case 81:
            return "OP_1";
        case 82:
            return "OP_2";
        case 83:
            return "OP_3";
        case 84:
            return "OP_4";
        case 85:
            return "OP_5";
        case 86:
            return "OP_6";
        case 87:
            return "OP_7";
        case 88:
            return "OP_8";
        case 89:
            return "OP_9";
        case 90:
            return "OP_10";
        case 91:
            return "OP_11";
        case 92:
            return "OP_12";
        case 93:
            return "OP_13";
        case 94:
            return "OP_14";
        case 95:
            return "OP_15";
        case 96:
            return "OP_16";
        case 97:
            return "OP_NOP";
        case 99:
            return "OP_IF";
        case 100:
            return "OP_NOTIF";
        case 105:
            return "OP_VERIFY";
        case 106:
            return "OP_RETURN";
        case 107:
            return "OP_TOALTSTACK";
        case 108:
            return "OP_FROMALTSTACK";
        case 109:
            return "OP_2DROP";
        case 110:
            return "OP_2DUP";
        case 111:
            return "OP_3DUP";
        case 112:
            return "OP_2OVER";
        case 113:
            return "OP_2ROT";
        case 114:
            return "OP_2SWAP";
        case 115:
            return "OP_IFDUP";
        case 116:
            return "OP_DEPTH";
        case 117:
            return "OP_DROP";
        case 118:
            return "OP_DUP";
        case 119:
            return "OP_NIP";
        case 120:
            return "OP_OVER";
        case 121:
            return "OP_PICK";
        case 122:
            return "OP_ROLL";
        case 123:
            return "OP_ROT";
        case 124:
            return "OP_SWAP";
        case 125:
            return "OP_TUCK";
        case 130:
            return "OP_SIZE";
        case 135:
            return "OP_EQUAL";
        case 136:
            return "OP_EQUALVERIFY";
        case 139:
            return "OP_1ADD";
        case 140:
            return "OP_1SUB";
        case 143:
            return "OP_NEGATE";
        case 144:
            return "OP_ABS";
        case 145:
            return "OP_NOT";
        case 146:
            return "OP_0NOTEQUAL";
        case 147:
            return "OP_ADD";
        case 148:
            return "OP_SUB";
        case 149:
            return "OP_MUL";
        case 154:
            return "OP_BOOLAND";
        case 155:
            return "OP_BOOLOR";
        case 156:
            return "OP_NUMEQUAL";
        case 157:
            return "OP_NUMEQUALVERIFY";
        case 158:
            return "OP_NUMNOTEQUAL";
        case 159:
            return "OP_LESSTHAN";
        case 160:
            return "OP_GREATERTHAN";
        case 161:
            return "OP_LESSTHANOREQUAL";
        case 162:
            return "OP_GREATERTHANOREQUAL";
        case 163:
            return "OP_MIN";
        case 164:
            return "OP_MAX";
        case 165:
            return "OP_WITHIN";
        case 166:
            return "OP_RIPEMD160";
        case 167:
            return "OP_SHA1";
        case 168:
            return "OP_SHA256";
        case 169:
            return "OP_HASH160";
        case 170:
            return "OP_HASH256";
        case 172:
            return "OP_CHECKSIG";
        case 173:
            return "OP_CHECKSIGVERIFY";
        case 174:
            return "OP_CHECKMULTISIG";
        case 175:
            return "OP_CHECKMULTISIGVERIFY";
        case 176:
            return "OP_NOP";
        case 177:
            return "OP_CHECKLOCKTIMEVERIFY";
        case 178:
            return "OP_CHECKSEQUENCEVERIFY";
        case 179:
            return "OP_NOP";
        case 180:
            return "OP_NOP";
        case 181:
            return "OP_NOP";
        case 182:
            return "OP_NOP";
        case 183:
            return "OP_NOP";
        case 184:
            return "OP_NOP";
        case 185:
            return "OP_NOP";
        default:
            return "UNKNOWN";
    }
}

void op_perform_operation_basic(Op* op, int cmd_int) {
    switch (cmd_int) {
        case 0:
            op_0(op);
            break;
        case 79:
            op_1negate(op);
            break;
        case 81:
            op_1(op);
            break;
        case 82:
            op_2(op);
            break;
        case 83:
            op_3(op);
            break;
        case 84:
            op_4(op);
            break;
        case 85:
            op_5(op);
            break;
        case 86:
            op_6(op);
            break;
        case 87:
            op_7(op);
            break;
        case 88:
            op_8(op);
            break;
        case 89:
            op_9(op);
            break;
        case 90:
            op_10(op);
            break;
        case 91:
            op_11(op);
            break;
        case 92:
            op_12(op);
            break;
        case 93:
            op_13(op);
            break;
        case 94:
            op_14(op);
            break;
        case 95:
            op_15(op);
            break;
        case 96:
            op_16(op);
            break;
        case 97:
            op_nop(op);
            break;
        case 105:
            op_verify(op);
            break;
        case 106:
            op_return(op);
            break;
        case 116:
            op_depth(op);
            break;
        case 117:
            op_drop(op);
            break;
        case 118:
            op_dup(op);
            break;
        case 169:
            op_hash160(op);
            break;
        case 168:
            op_sha256(op);
            break;
        default:
            printf("Unknown op code\n");
            exit(1);
    }
}

void op_perform_operation_z(Op* op, int cmd_int, S256Field* z) {
    switch (cmd_int) {
        case 172:
            op_checksig(op, z);
            break;
        default:
            printf("Unknown op code\n");
            exit(1);
    }
}