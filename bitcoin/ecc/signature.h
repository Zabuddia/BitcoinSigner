#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <stdio.h>
#include <stdlib.h>

#include "s256field.h"

typedef struct {
    S256Field* r;
    S256Field* s;
} Signature;

Signature* Signature_init(S256Field* r, S256Field* s);

void Signature_free(Signature* sig);

void Signature_toString(Signature* sig);

void Signature_der(Signature* sig, unsigned char* output);

Signature* Signature_parse(unsigned char* s);

#endif //SIGNATURE_H