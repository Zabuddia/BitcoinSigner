#ifndef OP_H
#define OP_H

#include "../helper/helper.h"

#define STACK_SIZE 1000

typedef struct {
    unsigned char stack[STACK_SIZE][STACK_SIZE];
    size_t element_length[STACK_SIZE];
    int top;
} Op;

Op* op_init();

void op_free(Op* op);

void push(Op* op, unsigned char* element, size_t element_len);

void pop(Op* op, unsigned char* result);

void peek(Op* op, unsigned char* result);

void encode_num(long long num, unsigned char* result);

long long decode_num(unsigned char* element, size_t element_len);

size_t op_0(Op* op);

size_t op_1negate(Op* op);

size_t op_1(Op* op);

size_t op_2(Op* op);

size_t op_3(Op* op);

size_t op_4(Op* op);

size_t op_5(Op* op);

size_t op_6(Op* op);

size_t op_7(Op* op);

size_t op_8(Op* op);

size_t op_9(Op* op);

size_t op_10(Op* op);

size_t op_11(Op* op);

size_t op_12(Op* op);

size_t op_13(Op* op);

size_t op_14(Op* op);

size_t op_15(Op* op);

size_t op_16(Op* op);

size_t op_nop(Op* op);

size_t op_verify(Op* op);

size_t op_return(Op* op);

size_t op_depth(Op* op);

size_t op_drop(Op* op);

size_t op_dup(Op* op);

size_t op_hash160(Op* op);

size_t op_ripemd160(Op* op);

size_t op_sha256(Op* op);

size_t op_hash256(Op* op);

#endif // OP_H