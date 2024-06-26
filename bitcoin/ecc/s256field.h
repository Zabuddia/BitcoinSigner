#ifndef S256FIELD_H
#define S256FIELD_H

#include <math.h>
#include <assert.h>

#include "../helper/helper.h"

typedef struct {
    mpz_t num;
    mpz_t prime;
} S256Field;

void Initialize_prime();

void Free_prime();

S256Field* S256Field_init(mpz_t num);

void S256Field_free(S256Field* element);

void S256Field_toString(S256Field* element);

bool S256Field_eq(S256Field* e1, S256Field* e2);

bool S256Field_ne(S256Field* e1, S256Field* e2);

S256Field* S256Field_add(S256Field* e1, S256Field* e2);

S256Field* S256Field_sub(S256Field* e1, S256Field* e2);

S256Field* S256Field_mul(S256Field* e1, S256Field* e2);

S256Field* S256Field_s_mul(S256Field* e1, S256Field* e2);

S256Field* S256Field_s_mul_scalar(S256Field* e, mpz_t s);

S256Field* S256Field_mul_scalar(S256Field* e, mpz_t s);

S256Field* S256Field_mod_inv(S256Field* e);

S256Field* S256Field_s_inv(S256Field* e);

S256Field* S256Field_pow(S256Field* e1, mpz_t exponent);

S256Field* S256Field_div(S256Field* e1, S256Field* e2);

S256Field* S256Field_sqrt(S256Field* e);

#endif //S256FIELD_H