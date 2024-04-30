#ifndef OP_H
#define OP_H

#include "../helper/helper.h"
#include "../ecc/s256point.h"

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

size_t encode_num(long long num, unsigned char* result);

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

size_t op_checksig(Op* op, S256Field* z);

size_t op_if(Op* op);

size_t op_notif(Op* op);

size_t op_toaltstack(Op* op, Op* altstack);

size_t op_fromaltstack(Op* op, Op* altstack);

size_t op_2drop(Op* op);

size_t op_2dup(Op* op);

size_t op_3dup(Op* op);

size_t op_2over(Op* op);

size_t op_2rot(Op* op);

size_t op_2swap(Op* op);

size_t op_ifdup(Op* op);

size_t op_nip(Op* op);

size_t op_over(Op* op);

size_t op_pick(Op* op);

size_t op_roll(Op* op);

size_t op_rot(Op* op);

size_t op_swap(Op* op);

size_t op_tuck(Op* op);

size_t op_size(Op* op);

size_t op_equal(Op* op);

size_t op_mul(Op* op);

size_t op_equalverify(Op* op);

size_t op_1add(Op* op);

size_t op_1sub(Op* op);

size_t op_negate(Op* op);

size_t op_abs(Op* op);

size_t op_not(Op* op);

size_t op_0notequal(Op* op);

size_t op_add(Op* op);

size_t op_sub(Op* op);

size_t op_booland(Op* op);

size_t op_boolor(Op* op);

size_t op_numequal(Op* op);

size_t op_numequalverify(Op* op);

size_t op_numnotequal(Op* op);

size_t op_lessthan(Op* op);

size_t op_greaterthan(Op* op);

size_t op_lessthanorequal(Op* op);

size_t op_greaterthanorequal(Op* op);

size_t op_min(Op* op);

size_t op_max(Op* op);

size_t op_within(Op* op);

size_t op_sha1(Op* op);

size_t op_checksigverify(Op* op, S256Field* z);

size_t op_checkmultisig(Op* op, S256Field* z);

size_t op_checkmultisigverify(Op* op, S256Field* z);

size_t op_checklocktimeverify(Op* op);

size_t op_checksequenceverify(Op* op);

char* op_code_functions(int cmd_int);

void op_perform_operation_basic(Op* op, int cmd_int);

void op_perform_operation_z(Op* op, int cmd_int, S256Field* z);

void op_perform_operation_alt(Op* op, int cmd_int, Op* altstack);

#endif // OP_H