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

typedef struct {
    mpz_t secret;
    S256Field* e;
    S256Point* point;
} PrivateKeyInt;

PrivateKey* PrivateKey_init_str(const char* secret);

PrivateKeyInt* PrivateKey_init_int(mpz_t secret);

//Not sure if this does the right thing
S256Field* Deterministic_k(PrivateKey* key, S256Field* z);

Signature* PrivateKey_sign(PrivateKey* key, S256Field* z);

#endif //PRIVATEKEY_H