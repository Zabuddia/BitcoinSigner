#ifndef OP_H
#define OP_H

#include "../helper/helper.h"

#define STACK_SIZE 1000

void push(unsigned char* element);

unsigned char* pop();

unsigned char* peek();

void encode_num(long long num, unsigned char* result);

long long decode_num(unsigned char* element, size_t element_len);

#endif // OP_H