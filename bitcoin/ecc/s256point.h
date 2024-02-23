#ifndef S256POINT_H
#define S256POINT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "signature.h"
#include "../helper.h"

typedef struct {
    S256Field* x;
    S256Field* y;
    S256Field* a;
    S256Field* b;
} S256Point;

void Initialize_G();

void Free_G();

S256Point* S256Point_init(S256Field* x, S256Field* y);

void S256Point_free(S256Point* p);

void S256Point_toString(S256Point* p);

int S256Point_eq(S256Point* p1, S256Point* p2);

int S256Point_ne(S256Point* p1, S256Point* p2);

S256Point* S256Point_add(S256Point* p1, S256Point* p2);

S256Point* S256Point_mul(S256Point* p1, mpz_t coefficient);

int S256Point_verify(S256Point* p, S256Field* z, Signature* sig);

void S256Point_sec_uncompressed(S256Point* p, unsigned char* output);

void S256Point_sec_compressed(S256Point* p, unsigned char* output);

S256Point* S256Point_parse_sec(S256Point* p, unsigned char* sec_bin);

#endif //S256POINT_H