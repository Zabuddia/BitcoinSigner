#include "op.h"

Op* Op_init() {
    Op* op = malloc(sizeof(Op));
    op->top = -1;
    for (int i = 0; i < STACK_SIZE; i++) {
        memset(op->stack[i], 0, sizeof(op->stack[i]));
        op->element_length[i] = 0;
    }
    return op;
}

void Op_free(Op* op) {
    free(op);
}

void push(Op* op, uint8_t* element, uint32_t element_len) {
    if (op->top == STACK_SIZE - 1) {
        printf("Op stack overflow\n");
        exit(1);
    }
    op->top++;
    memcpy(op->stack[op->top], element, element_len);
    op->element_length[op->top] = element_len;
}

void pop(Op* op, uint8_t* result) {
    if (op->top == -1) {
        printf("Op stack underflow\n");
        exit(1);
    }
    uint32_t element_len = op->element_length[op->top];
    memcpy(result, op->stack[op->top], element_len);
    memset(op->stack[op->top], 0, element_len);
    op->top--;
}

void peek(Op* op, uint8_t* result) {
    if (op->top == -1) {
        printf("Op stack underflow\n");
        exit(1);
    }
    uint32_t element_len = op->element_length[op->top];
    memcpy(result, op->stack[op->top], element_len);
}

uint32_t encode_num(int64_t num, uint8_t* result) {
    if (num == 0) {
        result[0] = 0;
        return 1;
    }
    int64_t abs_num = llabs(num);
    bool negative = num < 0 ? true : false;
    uint32_t result_len = 0;
    while (abs_num > 0) {
        result[result_len++] = abs_num & 0xff;
        abs_num >>= 8;
    }
    if (result[result_len - 1] & 0x80) {
        result[result_len++] = negative ? 0x80 : 0;
    } else if (negative) {
        result[result_len - 1] |= 0x80;
    }
    return result_len;
}

int64_t decode_num(uint8_t* element, uint32_t element_len) {
    if (element[0] == 0) {
        return 0;
    }
    int64_t result = 0;
    little_endian_to_big_endian(element, element_len);
    bool negative = false;
    if (element[0] & 0x80) {
        negative = true;
        result = element[0] & 0x7f;
    } else {
        negative = false;
        result = element[0];
    }
    for (int32_t i = 1; i < element_len; i++) {
        result <<= 8;
        result += element[i];
    }
    if (negative) {
        result = -result;
    }
    little_endian_to_big_endian(element, element_len);
    return result;
}

bool Op_0(Op* op) {
    uint8_t element[1];
    encode_num(0, element);
    push(op, element, 1);
    return true;
}

bool Op_1negate(Op* op) {
    uint8_t element[1];
    encode_num(-1, element);
    push(op, element, 1);
    return true;
}

bool Op_1(Op* op) {
    uint8_t element[1];
    encode_num(1, element);
    push(op, element, 1);
    return true;
}

bool Op_2(Op* op) {
    uint8_t element[1];
    encode_num(2, element);
    push(op, element, 1);
    return true;
}

bool Op_3(Op* op) {
    uint8_t element[1];
    encode_num(3, element);
    push(op, element, 1);
    return true;
}

bool Op_4(Op* op) {
    uint8_t element[1];
    encode_num(4, element);
    push(op, element, 1);
    return true;
}

bool Op_5(Op* op) {
    uint8_t element[1];
    encode_num(5, element);
    push(op, element, 1);
    return true;
}

bool Op_6(Op* op) {
    uint8_t element[1];
    encode_num(6, element);
    push(op, element, 1);
    return true;
}

bool Op_7(Op* op) {
    uint8_t element[1];
    encode_num(7, element);
    push(op, element, 1);
    return true;
}

bool Op_8(Op* op) {
    uint8_t element[1];
    encode_num(8, element);
    push(op, element, 1);
    return true;
}

bool Op_9(Op* op) {
    uint8_t element[1];
    encode_num(9, element);
    push(op, element, 1);
    return true;
}

bool Op_10(Op* op) {
    uint8_t element[1];
    encode_num(10, element);
    push(op, element, 1);
    return true;
}

bool Op_11(Op* op) {
    uint8_t element[1];
    encode_num(11, element);
    push(op, element, 1);
    return true;
}

bool Op_12(Op* op) {
    uint8_t element[1];
    encode_num(12, element);
    push(op, element, 1);
    return true;
}

bool Op_13(Op* op) {
    uint8_t element[1];
    encode_num(13, element);
    push(op, element, 1);
    return true;
}

bool Op_14(Op* op) {
    uint8_t element[1];
    encode_num(14, element);
    push(op, element, 1);
    return true;
}

bool Op_15(Op* op) {
    uint8_t element[1];
    encode_num(15, element);
    push(op, element, 1);
    return true;
}

bool Op_16(Op* op) {
    uint8_t element[1];
    encode_num(16, element);
    push(op, element, 1);
    return true;
}

bool Op_nop(Op* op) {
    return true;
}

bool Op_verify(Op* op) {
    uint8_t element[op->element_length[op->top]];
    pop(op, element);
    long long num = decode_num(element, op->element_length[op->top]);
    if (num == 0) {
        return false;
    }
    return true;
}

bool Op_return(Op* op) {
    return false;
}

bool Op_depth(Op* op) {
    uint8_t element[1];
    encode_num(op->top + 1, element);
    push(op, element, 1);
    return true;
}

bool Op_drop(Op* op) {
    uint8_t element[op->element_length[op->top]];
    pop(op, element);
    return true;
}

bool Op_dup(Op* op) {
    if (op->top == -1) {
        return false;
    }
    uint8_t element[op->element_length[op->top]];
    memcpy(element, op->stack[op->top], op->element_length[op->top]);
    push(op, element, op->element_length[op->top]);
    return true;
}

bool Op_hash160(Op* op) {
    if (op->top < 0) {
        return false;
    }
    uint8_t element[op->element_length[op->top]];
    uint32_t element_len = op->element_length[op->top];
    pop(op, element);
    uint8_t hash[20];
    hash160(element, element_len, hash);
    push(op, hash, 20);
    return true;
}

bool Op_ripemd160(Op* op) {
    uint8_t element[op->element_length[op->top]];
    uint32_t element_len = op->element_length[op->top];
    pop(op, element);
    uint8_t hash[20];
    ripemd160(element, element_len, hash);
    push(op, hash, 20);
    return true;
}

bool Op_sha256(Op* op) {
    uint8_t element[op->element_length[op->top]];
    uint32_t element_len = op->element_length[op->top];
    pop(op, element);
    uint8_t hash[32];
    sha256(element, element_len, hash);
    push(op, hash, 32);
    return true;
}

bool Op_hash256(Op* op) {
    uint8_t element[op->element_length[op->top]];
    uint32_t element_len = op->element_length[op->top];
    pop(op, element);
    uint8_t hash[32];
    hash256(element, element_len, hash);
    push(op, hash, 32);
    return true;
}

bool Op_checksig(Op* op, S256Field* z) {
    if (op->top < 1) {
        return false;
    }
    uint8_t sec_pubkey[op->element_length[op->top]];
    pop(op, sec_pubkey);
    uint8_t der_signature[op->element_length[op->top]];
    pop(op, der_signature);
    S256Point* point = S256Point_parse_sec(sec_pubkey);
    Signature* sig = Signature_parse(der_signature);
    bool result = S256Point_verify(point, z, sig);
    S256Point_free(point);
    Signature_free(sig);
    if (result) {
        uint8_t element[1];
        encode_num(1, element);
        push(op, element, 1);
    } else {
        uint8_t element[1];
        encode_num(0, element);
        push(op, element, 1);
    }
    return true;
}

bool Op_if(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_notif(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_toaltstack(Op* op, Op* altstack) {
    if (op->top < 0) {
        return false;
    }
    uint8_t element[op->element_length[op->top]];
    pop(op, element);
    push(altstack, element, op->element_length[op->top]);
    return true;
}

bool Op_fromaltstack(Op* op, Op* altstack) {
    if (altstack->top < 0) {
        return false;
    }
    uint8_t element[altstack->element_length[altstack->top]];
    pop(altstack, element);
    push(op, element, altstack->element_length[altstack->top]);
    return true;
}

bool Op_2drop(Op* op) {
    if (op->top < 1) {
        return false;
    }
    pop(op, NULL);
    pop(op, NULL);
    return true;
}

bool Op_2dup(Op* op) {
    if (op->top < 1) {
        return false;
    }
    uint8_t element_2[op->element_length[op->top]];
    pop(op, element_2);
    uint8_t element_1[op->element_length[op->top]];
    peek(op, element_1);
    push(op, element_2, op->element_length[op->top]);
    push(op, element_1, op->element_length[op->top]);
    push(op, element_2, op->element_length[op->top]);
    return true;
}

bool Op_3dup(Op* op) {
    if (op->top < 2) {
        return false;
    }
    uint8_t element1[op->element_length[op->top]];
    uint8_t element2[op->element_length[op->top]];
    peek(op, element1);
    peek(op, element2);
    push(op, element2, op->element_length[op->top]);
    push(op, element1, op->element_length[op->top]);
    push(op, element2, op->element_length[op->top]);
    return true;
}

bool Op_2over(Op* op) {
    if (op->top < 3) {
        return false;
    }
    uint8_t element1[op->element_length[op->top]];
    uint8_t element2[op->element_length[op->top]];
    peek(op, element1);
    peek(op, element2);
    push(op, element2, op->element_length[op->top]);
    push(op, element1, op->element_length[op->top]);
    return true;
}

bool Op_2rot(Op* op) {
    if (op->top < 5) {
        return false;
    }
    uint8_t element1[op->element_length[op->top]];
    uint8_t element2[op->element_length[op->top]];
    peek(op, element1);
    peek(op, element2);
    push(op, element2, op->element_length[op->top]);
    push(op, element1, op->element_length[op->top]);
    return true;
}

bool Op_2swap(Op* op) {
    if (op->top < 3) {
        return false;
    }
    uint8_t element1[op->element_length[op->top]];
    uint8_t element2[op->element_length[op->top]];
    peek(op, element1);
    peek(op, element2);
    push(op, element2, op->element_length[op->top]);
    push(op, element1, op->element_length[op->top]);
    return true;
}

bool Op_ifdup(Op* op) {
    if (op->top < 0) {
        return false;
    }
    uint8_t element[op->element_length[op->top]];
    peek(op, element);
    if (decode_num(element, op->element_length[op->top]) != 0) {
        push(op, element, op->element_length[op->top]);
    }
    return true;
}

bool Op_nip(Op* op) {
    if (op->top < 1) {
        return false;
    }
    uint8_t element[op->element_length[op->top]];
    pop(op, element);
    return true;
}

bool Op_over(Op* op) {
    if (op->top < 1) {
        return false;
    }
    uint8_t element[op->element_length[op->top]];
    peek(op, element);
    push(op, element, op->element_length[op->top]);
    return true;
}

bool Op_pick(Op* op) {
    if (op->top < 0) {
        return false;
    }
    uint8_t n_element[op->element_length[op->top]];
    pop(op, n_element);
    int64_t n = decode_num(n_element, op->element_length[op->top]);
    if (op->top < n) {
        return false;
    }
    uint8_t element[op->element_length[op->top]];
    peek(op, element);
    push(op, element, op->element_length[op->top]);
    return true;
}

bool Op_roll(Op* op) {
    if (op->top < 0) {
        return false;
    }
    uint8_t n_element[op->element_length[op->top]];
    pop(op, n_element);
    int64_t n = decode_num(n_element, op->element_length[op->top]);
    if (op->top < n) {
        return false;
    }
    if (n == 0) {
        return true;
    }
    uint8_t element[op->element_length[op->top]];
    peek(op, element);
    push(op, element, op->element_length[op->top]);
    return true;
}

bool Op_rot(Op* op) {
    if (op->top < 2) {
        return false;
    }
    uint8_t element[op->element_length[op->top]];
    peek(op, element);
    push(op, element, op->element_length[op->top]);
    return true;
}

bool Op_swap(Op* op) {
    if (op->top < 1) {
        return false;
    }
    uint8_t element_2[op->element_length[op->top]];
    pop(op, element_2);
    uint8_t element_1[op->element_length[op->top]];
    pop(op, element_1);
    push(op, element_2, op->element_length[op->top]);
    push(op, element_1, op->element_length[op->top]);
    return true;
}

bool Op_tuck(Op* op) {
    if (op->top < 1) {
        return false;
    }
    uint8_t element[op->element_length[op->top]];
    peek(op, element);
    push(op, element, op->element_length[op->top]);
    return true;
}

bool Op_size(Op* op) {
    if (op->top < 0) {
        return false;
    }
    uint8_t element[op->element_length[op->top]];
    peek(op, element);
    uint8_t size_element[STACK_SIZE];
    uint32_t size_len = encode_num(op->element_length[op->top], size_element);
    push(op, size_element, size_len);
    return true;
}

bool Op_equal(Op* op) {
    if (op->top < 1) {
        return false;
    }
    uint32_t element_len = op->element_length[op->top];
    uint8_t element_1[op->element_length[op->top]];
    memset(element_1, 0, op->element_length[op->top]);
    pop(op, element_1);
    uint8_t element_2[op->element_length[op->top]];
    memset(element_2, 0, op->element_length[op->top]);
    pop(op, element_2);
    if (memcmp(element_1, element_2, element_len)) {
        uint8_t result[1];
        encode_num(0, result);
        push(op, result, 1);
    } else {
        uint8_t result[1];
        encode_num(1, result);
        push(op, result, 1);
    }
    return true;
}

bool Op_mul(Op* op) {
    if (op->top < 1) {
        return false;
    }
    uint8_t element_1[op->element_length[op->top]];
    pop(op, element_1);
    uint8_t element_2[op->element_length[op->top]];
    pop(op, element_2);
    int64_t num_1 = decode_num(element_1, op->element_length[op->top]);
    int64_t num_2 = decode_num(element_2, op->element_length[op->top]);
    int64_t result = num_1 * num_2;
    uint8_t result_element[STACK_SIZE];
    uint32_t result_len = encode_num(result, result_element);
    push(op, result_element, result_len);
    return true;
}

bool Op_equalverify(Op* op) {
    return Op_equal(op) && Op_verify(op);
}

bool Op_1add(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_1sub(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_negate(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_abs(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_not(Op* op) {
    if (op->top < 0) {
        return false;
    }
    uint8_t element[op->element_length[op->top]];
    pop(op, element);
    int64_t num = decode_num(element, op->element_length[op->top]);
    uint8_t result[1];
    if (num == 0) {
        encode_num(1, result);
    } else {
        encode_num(0, result);
    }
    push(op, result, 1);
    return true;
}

bool Op_0notequal(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_add(Op* op) {
    if (op->top < 1) {
        return false;
    }
    uint8_t element_1[op->element_length[op->top]];
    pop(op, element_1);
    uint8_t element_2[op->element_length[op->top]];
    pop(op, element_2);
    int64_t num_1 = decode_num(element_1, op->element_length[op->top]);
    int64_t num_2 = decode_num(element_2, op->element_length[op->top]);
    int64_t result = num_1 + num_2;
    uint8_t result_element[STACK_SIZE];
    uint32_t result_len = encode_num(result, result_element);
    push(op, result_element, result_len);
    return true;
}

bool Op_sub(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_booland(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_boolor(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_numequal(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_numequalverify(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_numnotequal(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_lessthan(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_greaterthan(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_lessthanorequal(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_greaterthanorequal(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_min(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_max(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_within(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_sha1(Op* op) {
    if (op->top < 0) {
        return false;
    }
    uint8_t element[op->element_length[op->top]];
    uint32_t element_len = op->element_length[op->top];
    pop(op, element);
    uint8_t hash[20];
    sha1(element, element_len, hash);
    push(op, hash, 20);
    return true;
}

bool Op_checksigverify(Op* op, S256Field* z) {
    printf("Not implemented\n");
    return false;
}

bool Op_checkmultisig(Op* op, S256Field* z) {
    if (op->top < 0) {
        return false;
    }
    uint32_t n_element_length = op->element_length[op->top];
    uint8_t n_element[n_element_length];
    pop(op, n_element);
    int64_t n = decode_num(n_element, n_element_length);
    if (op->top < n) {
        return false;
    }
    uint8_t* sec_pubkeys[n];
    for (int32_t i = 0; i < n; i++) {
        sec_pubkeys[i] = malloc(op->element_length[op->top]);
        pop(op, sec_pubkeys[i]);
    }
    uint32_t m_element_length = op->element_length[op->top];
    uint8_t m_element[m_element_length];
    pop(op, m_element);
    int64_t m = decode_num(m_element, m_element_length);
    if (op->top < m) {
        return false;
    }
    uint8_t* der_signatures[m];
    for (int32_t i = 0; i < m; i++) {
        der_signatures[i] = malloc(op->element_length[op->top]);
        pop(op, der_signatures[i]);
    }
    S256Point* points[n];
    for (int32_t i = 0; i < n; i++) {
        points[i] = S256Point_parse_sec(sec_pubkeys[i]);
    }
    Signature* sigs[m];
    for (int32_t i = 0; i < m; i++) {
        sigs[i] = Signature_parse(der_signatures[i]);
    }
    uint8_t element[1];
    pop(op, element);
    for (int32_t i = 0; i < m; i++) {
        bool found = 0;
        for (int32_t j = 0; j < n; j++) {
            if (S256Point_verify(points[j], z, sigs[i])) {
                found = true;
                break;
            }
        }
        if (!found) {
            for (int32_t j = 0; j < n; j++) {
                S256Point_free(points[j]);
                free(sec_pubkeys[j]);
            }
            for (int32_t j = 0; j < m; j++) {
                Signature_free(sigs[j]);
                free(der_signatures[j]);
            }
            return false;
        }
    }
    uint8_t result_element[1];
    encode_num(1, result_element);
    push(op, result_element, 1);
    for (int32_t i = 0; i < n; i++) {
        S256Point_free(points[i]);
        free(sec_pubkeys[i]);
    }
    for (int32_t i = 0; i < m; i++) {
        Signature_free(sigs[i]);
        free(der_signatures[i]);
    }
    return true;
}

bool Op_checkmultisigverify(Op* op, S256Field* z) {
    printf("Not implemented\n");
    return false;
}

bool Op_checklocktimeverify(Op* op) {
    printf("Not implemented\n");
    return false;
}

bool Op_checksequenceverify(Op* op) {
    printf("Not implemented\n");
    return false;
}

char* Op_code_functions(uint8_t cmd_int) {
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

void Op_perform_operation_basic(Op* op, uint8_t cmd_int) {
    switch (cmd_int) {
        case 0:
            Op_0(op);
            break;
        case 79:
            Op_1negate(op);
            break;
        case 81:
            Op_1(op);
            break;
        case 82:
            Op_2(op);
            break;
        case 83:
            Op_3(op);
            break;
        case 84:
            Op_4(op);
            break;
        case 85:
            Op_5(op);
            break;
        case 86:
            Op_6(op);
            break;
        case 87:
            Op_7(op);
            break;
        case 88:
            Op_8(op);
            break;
        case 89:
            Op_9(op);
            break;
        case 90:
            Op_10(op);
            break;
        case 91:
            Op_11(op);
            break;
        case 92:
            Op_12(op);
            break;
        case 93:
            Op_13(op);
            break;
        case 94:
            Op_14(op);
            break;
        case 95:
            Op_15(op);
            break;
        case 96:
            Op_16(op);
            break;
        case 97:
            Op_nop(op);
            break;
        case 99:
            Op_if(op);
            break;
        case 100:
            Op_notif(op);
            break;
        case 105:
            Op_verify(op);
            break;
        case 106:
            Op_return(op);
            break;
        case 109:
            Op_2drop(op);
            break;
        case 110:
            Op_2dup(op);
            break;
        case 111:
            Op_3dup(op);
            break;
        case 112:
            Op_2over(op);
            break;
        case 113:
            Op_2rot(op);
            break;
        case 114:
            Op_2swap(op);
            break;
        case 115:
            Op_ifdup(op);
            break;
        case 116:
            Op_depth(op);
            break;
        case 117:
            Op_drop(op);
            break;
        case 118:
            Op_dup(op);
            break;
        case 119:
            Op_nip(op);
            break;
        case 120:
            Op_over(op);
            break;
        case 121:
            Op_pick(op);
            break;
        case 122:
            Op_roll(op);
            break;
        case 123:
            Op_rot(op);
            break;
        case 124:
            Op_swap(op);
            break;
        case 125:
            Op_tuck(op);
            break;
        case 130:
            Op_size(op);
            break;
        case 135:
            Op_equal(op);
            break;
        case 136:
            Op_equalverify(op);
            break;
        case 139:
            Op_1add(op);
            break;
        case 140:
            Op_1sub(op);
            break;
        case 143:
            Op_negate(op);
            break;
        case 144:
            Op_abs(op);
            break;
        case 145:
            Op_not(op);
            break;
        case 146:
            Op_0notequal(op);
            break;
        case 147:
            Op_add(op);
            break;
        case 148:
            Op_sub(op);
            break;
        case 149:
            Op_mul(op);
            break;
        case 154:
            Op_booland(op);
            break;
        case 155:
            Op_boolor(op);
            break;
        case 156:
            Op_numequal(op);
            break;
        case 157:
            Op_numequalverify(op);
            break;
        case 158:
            Op_numnotequal(op);
            break;
        case 159:
            Op_lessthan(op);
            break;
        case 160:
            Op_greaterthan(op);
            break;
        case 161:
            Op_lessthanorequal(op);
            break;
        case 162:
            Op_greaterthanorequal(op);
            break;
        case 163:
            Op_min(op);
            break;
        case 164:
            Op_max(op);
            break;
        case 165:
            Op_within(op);
            break;
        case 166:
            Op_ripemd160(op);
            break;
        case 167:
            Op_sha1(op);
            break;
        case 168:
            Op_sha256(op);
            break;
        case 169:
            Op_hash160(op);
            break;
        case 170:
            Op_hash256(op);
            break;
        case 176:
            Op_nop(op);
            break;
        case 177:
            Op_checklocktimeverify(op);
            break;
        case 178:
            Op_checksequenceverify(op);
            break;
        case 179:
            Op_nop(op);
            break;
        case 180:
            Op_nop(op);
            break;
        case 181:
            Op_nop(op);
            break;
        case 182:
            Op_nop(op);
            break;
        case 183:
            Op_nop(op);
            break;
        case 184:
            Op_nop(op);
            break;
        case 185:
            Op_nop(op);
            break;
        default:
            printf("Unknown op code\n");
            exit(1);
    }
}

void Op_perform_operation_z(Op* op, uint8_t cmd_int, S256Field* z) {
    switch (cmd_int) {
        case 172:
            Op_checksig(op, z);
            break;
        case 173:
            Op_checksigverify(op, z);
            break;
        case 174:
            Op_checkmultisig(op, z);
            break;
        case 175:
            Op_checkmultisigverify(op, z);
            break;
        default:
            printf("Unknown op code\n");
            exit(1);
    }
}

void Op_perform_operation_alt(Op* op, uint8_t cmd_int, Op* altstack) {
    switch (cmd_int) {
        case 107:
            Op_toaltstack(op, altstack);
            break;
        case 108:
            Op_fromaltstack(op, altstack);
            break;
        default:
            printf("Unknown op code\n");
            exit(1);
    }
}