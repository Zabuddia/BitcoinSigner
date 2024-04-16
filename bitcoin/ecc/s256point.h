#ifndef S256POINT_H
#define S256POINT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "signature.h"
#include "../helper/helper.h"

typedef struct {
    S256Field* x;
    S256Field* y;
    S256Field* a;
    S256Field* b;
} S256Point;

S256Point* S256Point_init(S256Field* x, S256Field* y);

S256Point* S256Point_deepcopy(S256Point* p);

void S256Point_half_deepcopy(S256Point* p, S256Point* p_copy);

void S256Point_free(S256Point* p);

void S256Point_half_free(S256Point* p);

void S256Point_toString(S256Point* p);

int S256Point_eq(S256Point* p1, S256Point* p2);

int S256Point_ne(S256Point* p1, S256Point* p2);

S256Point* S256Point_add(S256Point* p1, S256Point* p2);

S256Point* S256Point_mul(S256Point* p1, mpz_t coefficient);

int S256Point_verify(S256Point* p, S256Field* z, Signature* sig);

void S256Point_sec_uncompressed(S256Point* p, unsigned char* output);

void S256Point_sec_compressed(S256Point* p, unsigned char* output);

S256Point* S256Point_parse_sec(unsigned char* sec_bin);

void S256Point_hash160(S256Point* p, unsigned char* output, uint8_t compressed);

void S256Point_address(S256Point* p, unsigned char* output, uint8_t compressed, uint8_t testnet);

#endif //S256POINT_H