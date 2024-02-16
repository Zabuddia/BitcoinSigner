#ifndef S256FIELD_H
#define S256FIELD_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gmp.h>

#define A 0
#define B 7
#define P "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"
#define N "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141"

typedef struct {
    mpz_t num;
    mpz_t prime;
} S256Field;



#endif //S256FIELD_H