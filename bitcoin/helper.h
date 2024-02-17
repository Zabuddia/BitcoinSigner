#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#include "ecc/s256field.h"

S256Field* hash_to_s256field(const unsigned char* data, size_t data_len);

void mpz_to_bytes(const mpz_t op, unsigned char *out, size_t out_len);

void compute_hmac_sha256(unsigned char *key, int key_len,
                         unsigned char *data, int data_len,
                         unsigned char *output, size_t *output_len);

#endif //HELPER_H