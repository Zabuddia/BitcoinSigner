#ifndef S256FIELD_H
#define S256FIELD_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gmp.h>

#define P "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"
#define N "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141"

extern mpz_t PRIME;

typedef struct {
    mpz_t num;
    mpz_t prime;
} S256Field;

void Initialize_prime();

S256Field* S256Field_init(mpz_t num);

void S256Field_free(S256Field* element);

void S256Field_toString(S256Field* element);

int S256Field_eq(S256Field* e1, S256Field* e2);

int S256Field_ne(S256Field* e1, S256Field* e2);

S256Field* S256Field_add(S256Field* e1, S256Field* e2);

S256Field* S256Field_sub(S256Field* e1, S256Field* e2);

S256Field* S256Field_mul(S256Field* e1, S256Field* e2);

S256Field* S256Field_s_mul(S256Field* e1, S256Field* e2);

S256Field* S256Field_mul_scalar(S256Field* e, mpz_t s);

S256Field* S256Field_mod_inv(S256Field* e);

S256Field* S256Field_s_inv(S256Field* e);

S256Field* S256Field_pow(S256Field* e1, mpz_t exponent);

S256Field* S256Field_div(S256Field* e1, S256Field* e2);

#endif //S256FIELD_H