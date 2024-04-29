#include "op.h"

int top = -1;

unsigned char* stack[STACK_SIZE];

void push(unsigned char* element) {
    if (top == STACK_SIZE - 1) {
        printf("Op stack overflow\n");
        exit(1);
    }
    stack[++top] = element;
}

unsigned char* pop() {
    if (top == -1) {
        printf("Op stack underflow\n");
        exit(1);
    }
    return stack[top--];
}

unsigned char* peek() {
    if (top == -1) {
        printf("Op stack underflow\n");
        exit(1);
    }
    return stack[top];
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