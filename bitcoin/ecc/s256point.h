#ifndef S256POINT_H
#define S256POINT_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "signature.h"

#define GX "79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"
#define GY "483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"

extern mpz_t A;
extern mpz_t B;
extern mpz_t gx;
extern mpz_t gy;

typedef struct {
    S256Field* x;
    S256Field* y;
    S256Field* a;
    S256Field* b;
} S256Point;

void Initialize_G();

void Free_G();

void Initialize_a_and_b();

void Free_a_and_b();

S256Point* S256Point_init(S256Field* x, S256Field* y);

void S256Point_free(S256Point* p);

void S256Point_toString(S256Point* p);

int S256Point_eq(S256Point* p1, S256Point* p2);

int S256Point_ne(S256Point* p1, S256Point* p2);

S256Point* S256Point_add(S256Point* p1, S256Point* p2);

S256Point* S256Point_mul(S256Point* p1, mpz_t coefficient);

int S256Point_verify(S256Point* p, S256Field* z, Signature* sig);

#endif //S256POINT_H