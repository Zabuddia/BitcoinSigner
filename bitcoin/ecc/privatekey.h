#ifndef PRIVATEKEY_H
#define PRIVATEKEY_H

#include <string.h>
#include <openssl/evp.h>

#include "s256point.h"
#include "signature.h"
#include "../helper.h"

typedef struct {
    const char* secret;
    S256Field* e;
    S256Point* point;
} PrivateKey;

PrivateKey* PrivateKey_init(const char* secret);

Signature* PrivateKey_sign(PrivateKey* key, S256Field* z);

#endif //PRIVATEKEY_H