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

uint8_t Signature_der_length(Signature* sig);

void Signature_der(Signature* sig, uint8_t* output);

Signature* Signature_parse(uint8_t* s);

#endif //SIGNATURE_H