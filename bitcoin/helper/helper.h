#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <openssl/evp.h>
#include <sys/types.h>

#define P "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"
#define N "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141"
#define GX "79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"
#define GY "483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"

extern mpz_t PRIME;
extern mpz_t A;
extern mpz_t B;
extern mpz_t gx;
extern mpz_t gy;

void hash_to_mpz_t(const unsigned char* data, size_t data_len, mpz_t res);

void mpz_to_bytes(const mpz_t op, unsigned char *out, size_t out_len);

void mpz_to_32bytes(mpz_t num, unsigned char *output);

void compute_hmac_sha256(unsigned char *key, int key_len,
                         unsigned char *data, int data_len,
                         unsigned char *output, size_t *output_len);

void memzero(void* const pnt, const size_t len);

void encode_base58(char *b58, size_t *b58sz, const void *data, size_t binsz);

#endif //HELPER_H