#ifndef PRIVATEKEY_H
#define PRIVATEKEY_H

#include <string.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include "s256point.h"
#include "signature.h"
#include "../helper/helper.h"

typedef struct {
    const char* secret;
    S256Field* e;
    S256Point* point;
} PrivateKey;

// typedef struct {
//     mpz_t secret;
//     S256Field* e;
//     S256Point* point;
// } PrivateKey;

PrivateKey* PrivateKey_init(const char* secret);

// PrivateKey* PrivateKey_init(mpz_t secret);

void PrivateKey_free(PrivateKey* key);

//Not sure if this does the right thing
S256Field* Deterministic_k(PrivateKey* key, S256Field* z);

Signature* PrivateKey_sign(PrivateKey* key, S256Field* z);

void PrivateKey_wif(PrivateKey* key, unsigned char* output, uint8_t compressed, uint8_t testnet);

#endif //PRIVATEKEY_H