#ifndef PRIVATEKEY_H
#define PRIVATEKEY_H

#include <string.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include "s256point.h"
#include "signature.h"
#include "../helper.h"

typedef struct {
    const char* secret;
    S256Field* e;
    S256Point* point;
} PrivateKey;

PrivateKey* PrivateKey_init(const char* secret);

void mpz_to_bytes(const mpz_t op, unsigned char *out, size_t out_len);

void compute_hmac_sha256(unsigned char *key, int key_len,
                         unsigned char *data, int data_len,
                         unsigned char *output, size_t *output_len);

//Not sure if this does the right thing
S256Field* Deterministic_k(PrivateKey* key, S256Field* z);

Signature* PrivateKey_sign(PrivateKey* key, S256Field* z);

#endif //PRIVATEKEY_H