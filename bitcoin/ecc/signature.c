#include <stdio.h>
#include <stdlib.h>

#include "signature.h"

Signature* Signature_init(S256Field* r, S256Field* s) {
    Signature* sig = (Signature*)malloc(sizeof(Signature));
    if (sig == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    sig->r = r;
    sig->s = s;
    return sig;
}

void Signature_free(Signature* sig) {
    S256Field_free(sig->r);
    S256Field_free(sig->s);
    free(sig);
}

void Signature_toString(Signature* sig) {
    gmp_printf("Signature(%Zd, %Zd)\n", sig->r->num, sig->s->num);
}