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

int FieldElement_eq(FieldElement* e1, FieldElement* e2);

int FieldElement_ne(FieldElement* e1, FieldElement* e2);

FieldElement* FieldElement_add(FieldElement* e1, FieldElement* e2);

FieldElement* FieldElement_sub(FieldElement* e1, FieldElement* e2);

FieldElement* FieldElement_mul(FieldElement* e1, FieldElement* e2);

FieldElement* FieldElement_mul_scalar(FieldElement* e, mpz_t s);

FieldElement* FieldElement_mod_inv(FieldElement* e1);

FieldElement* FieldElement_pow(FieldElement* e1, mpz_t exponent);

FieldElement* FieldElement_div(FieldElement* e1, FieldElement* e2);

#endif //FIELDELEMENT_H