#ifndef FIELDELEMENTOLD_H
#define FIELDELEMENTOLD_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    int num;
    int prime;
} FieldElementOld;

FieldElementOld* FieldElementOld_init(int num, int prime);

void FieldElementOld_free(FieldElementOld* element);

void FieldElementOld_toString(FieldElementOld* element);

int FieldElementOld_eq(FieldElementOld* self, FieldElementOld* other);

int FieldElementOld_ne(FieldElementOld* self, FieldElementOld* other);

FieldElementOld* FieldElementOld_add(FieldElementOld* self, FieldElementOld* other);

FieldElementOld* FieldElementOld_sub(FieldElementOld* self, FieldElementOld* other);

FieldElementOld* FieldElementOld_mul(FieldElementOld* self, FieldElementOld* other);

FieldElementOld* FieldElementOld_mul_scalar(FieldElementOld* e, int s);

FieldElementOld* FieldElementOld_mod_inv(FieldElementOld* self);

FieldElementOld* FieldElementOld_pow(FieldElementOld* self, int exponent);

FieldElementOld* FieldElementOld_div(FieldElementOld* self, FieldElementOld* other);

#endif //FIELDELEMENTOLD_H