#ifndef OP_H
#define OP_H

#include "../helper/helper.h"
#include "../ecc/s256point.h"

#define STACK_SIZE 1000

typedef struct {
    uint8_t stack[STACK_SIZE][STACK_SIZE];
    uint32_t element_length[STACK_SIZE];
    int32_t top;
} Op;

Op* Op_init();

void Op_free(Op* op);

void push(Op* op, uint8_t* element, uint32_t element_len);

void pop(Op* op, uint8_t* result);

void peek(Op* op, uint8_t* result);

uint32_t encode_num(int64_t num, uint8_t* result);

int64_t decode_num(uint8_t* element, uint32_t element_len);

bool Op_0(Op* op);

bool Op_1negate(Op* op);

bool Op_1(Op* op);

bool Op_2(Op* op);

bool Op_3(Op* op);

bool Op_4(Op* op);

bool Op_5(Op* op);

bool Op_6(Op* op);

bool Op_7(Op* op);

bool Op_8(Op* op);

bool Op_9(Op* op);

bool Op_10(Op* op);

bool Op_11(Op* op);

bool Op_12(Op* op);

bool Op_13(Op* op);

bool Op_14(Op* op);

bool Op_15(Op* op);

bool Op_16(Op* op);

bool Op_nop(Op* op);

bool Op_verify(Op* op);

bool Op_return(Op* op);

bool Op_depth(Op* op);

bool Op_drop(Op* op);

bool Op_dup(Op* op);

bool Op_hash160(Op* op);

bool Op_ripemd160(Op* op);

bool Op_sha256(Op* op);

bool Op_hash256(Op* op);

bool Op_checksig(Op* op, S256Field* z);

bool Op_if(Op* op);

bool Op_notif(Op* op);

bool Op_toaltstack(Op* op, Op* altstack);

bool Op_fromaltstack(Op* op, Op* altstack);

bool Op_2drop(Op* op);

bool Op_2dup(Op* op);

bool Op_3dup(Op* op);

bool Op_2over(Op* op);

bool Op_2rot(Op* op);

bool Op_2swap(Op* op);

bool Op_ifdup(Op* op);

bool Op_nip(Op* op);

bool Op_over(Op* op);

bool Op_pick(Op* op);

bool Op_roll(Op* op);

bool Op_rot(Op* op);

bool Op_swap(Op* op);

bool Op_tuck(Op* op);

bool Op_size(Op* op);

bool Op_equal(Op* op);

bool Op_mul(Op* op);

bool Op_equalverify(Op* op);

bool Op_1add(Op* op);

bool Op_1sub(Op* op);

bool Op_negate(Op* op);

bool Op_abs(Op* op);

bool Op_not(Op* op);

bool Op_0notequal(Op* op);

bool Op_add(Op* op);

bool Op_sub(Op* op);

bool Op_booland(Op* op);

bool Op_boolor(Op* op);

bool Op_numequal(Op* op);

bool Op_numequalverify(Op* op);

bool Op_numnotequal(Op* op);

bool Op_lessthan(Op* op);

bool Op_greaterthan(Op* op);

bool Op_lessthanorequal(Op* op);

bool Op_greaterthanorequal(Op* op);

bool Op_min(Op* op);

bool Op_max(Op* op);

bool Op_within(Op* op);

bool Op_sha1(Op* op);

bool Op_checksigverify(Op* op, S256Field* z);

bool Op_checkmultisig(Op* op, S256Field* z);

bool Op_checkmultisigverify(Op* op, S256Field* z);

bool Op_checklocktimeverify(Op* op);

bool Op_checksequenceverify(Op* op);

char* Op_code_functions(uint8_t cmd_int);

void Op_perform_operation_basic(Op* op, uint8_t cmd_int);

void Op_perform_operation_z(Op* op, uint8_t cmd_int, S256Field* z);

void Op_perform_operation_alt(Op* op, uint8_t cmd_int, Op* altstack);

#endif // OP_H