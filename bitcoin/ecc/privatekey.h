#ifndef PRIVATEKEY_H
#define PRIVATEKEY_H

// Check if I need these
// #include <openssl/sha.h>
// #include <openssl/kdf.h>

#include "s256point.h"
#include "signature.h"
#include "../helper/helper.h"

typedef struct {
    S256Field* e;
    S256Point* point;
} PrivateKey;

PrivateKey* PrivateKey_init(mpz_t secret);

void PrivateKey_free(PrivateKey* key);

S256Field* Deterministic_k(PrivateKey* key, S256Field* z);

Signature* PrivateKey_sign(PrivateKey* key, S256Field* z);

void PrivateKey_wif(PrivateKey* key, uint8_t* output, bool compressed, bool testnet);

#endif //PRIVATEKEY_H