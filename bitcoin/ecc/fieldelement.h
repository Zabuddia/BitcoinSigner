#ifndef FIELDELEMENT_H
#define FIELDELEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gmp.h>

typedef struct {
    mpz_t num;
    mpz_t prime;
} FieldElement;

FieldElement* FieldElement_init(mpz_t num, mpz_t prime);

void FieldElement_free(FieldElement* element);

void FieldElement_toString(FieldElement* element);

int FieldElement_eq(FieldElement* self, FieldElement* other);

int FieldElement_ne(FieldElement* self, FieldElement* other);

FieldElement* FieldElement_add(FieldElement* self, FieldElement* other);

FieldElement* FieldElement_sub(FieldElement* self, FieldElement* other);

FieldElement* FieldElement_mul(FieldElement* self, FieldElement* other);

FieldElement* FieldElement_mul_scalar(FieldElement* e, mpz_t s);

FieldElement* FieldElement_mod_inv(FieldElement* self);

FieldElement* FieldElement_pow(FieldElement* self, mpz_t exponent);

FieldElement* FieldElement_div(FieldElement* self, FieldElement* other);

#endif //FIELDELEMENT_H