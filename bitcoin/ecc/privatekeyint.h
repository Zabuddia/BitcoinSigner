#ifndef PRIVATEKEYINT_H
#define PRIVATEKEYINT_H

#include <string.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include "s256point.h"
#include "signature.h"
#include "../helper.h"

typedef struct {
    mpz_t secret;
    S256Field* e;
    S256Point* point;
} PrivateKeyInt;

PrivateKeyInt* PrivateKeyInt_init(mpz_t secret);

//Not sure if this does the right thing
S256Field* PrivateKeyInt_deterministic_k(PrivateKeyInt* key, S256Field* z);

Signature* PrivateKeyInt_sign(PrivateKeyInt* key, S256Field* z);

#endif //PRIVATEKEYINT_H