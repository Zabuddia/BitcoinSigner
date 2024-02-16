#ifndef S256POINT_H
#define S256POINT_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "s256field.h"

#define A 0
#define B 7

typedef struct {
    S256Field* x;
    S256Field* y;
    S256Field* a;
    S256Field* b;
} S256Point;

S256Point* S256Point_init(S256Field* x, S256Field* y);

void S256Point_free(S256Point* p);

void S256Point_toString(S256Point* p);

int S256Point_eq(S256Point* p1, S256Point* p2);

int S256Point_ne(S256Point* p1, S256Point* p2);

S256Point* S256Point_add(S256Point* p1, S256Point* p2);

S256Point* S256Point_mul(S256Point* p1, mpz_t coefficient);

#endif //S256POINT_H