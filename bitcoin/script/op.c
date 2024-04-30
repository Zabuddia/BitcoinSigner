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