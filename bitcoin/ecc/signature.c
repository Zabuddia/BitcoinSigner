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
    gmp_printf("Signature(%Zx, %Zx)\n", sig->r->num, sig->s->num);
}

int Signature_der_length(Signature* sig) {
    size_t r_size = mpz_sizeinbase(sig->r->num, 256) + (mpz_sgn(sig->r->num) > 0 && mpz_tstbit(sig->r->num, mpz_size(sig->r->num) * GMP_NUMB_BITS - 1) ? 1 : 0);
    size_t s_size = mpz_sizeinbase(sig->s->num, 256) + (mpz_sgn(sig->s->num) > 0 && mpz_tstbit(sig->s->num, mpz_size(sig->s->num) * GMP_NUMB_BITS - 1) ? 1 : 0);
    return r_size + s_size + 6;
}

void Signature_der(Signature* sig, unsigned char* output) {
    if (!sig || !output) {
        printf("Invalid input\n");
        return;
    }

    int r_size = mpz_sizeinbase(sig->r->num, 256) + (mpz_sgn(sig->r->num) > 0 && mpz_tstbit(sig->r->num, mpz_size(sig->r->num) * GMP_NUMB_BITS - 1) ? 1 : 0);
    int s_size = mpz_sizeinbase(sig->s->num, 256) + (mpz_sgn(sig->s->num) > 0 && mpz_tstbit(sig->s->num, mpz_size(sig->s->num) * GMP_NUMB_BITS - 1) ? 1 : 0);

    int total_bytes = 2 /* header and length */ + 2 * (2 /* integer marker and size */) + r_size + s_size;

    size_t index = 0;
    output[index++] = 0x30; // DER sequence
    int_to_little_endian(total_bytes - 2, output + index, 1);
    index++;

    output[index++] = 0x02; // DER integer
    int_to_little_endian(r_size, output + index, 1);
    index++;

    unsigned char r_bin[r_size];
    memset(r_bin, 0, r_size);
    mpz_to_bytes(sig->r->num, r_bin, r_size);
    if (r_bin[0] & 0x80) {
        output[index++] = 0x00; // Add padding byte
    }
    memcpy(output + index, r_bin, r_size);
    index += r_size;

    output[index++] = 0x02; // DER integer
    int_to_little_endian(s_size, output + index, 1);
    index++;

    unsigned char s_bin[s_size];
    memset(s_bin, 0, s_size);
    mpz_to_bytes(sig->s->num, s_bin, s_size);
    if (s_bin[0] & 0x80) {
        output[index++] = 0x00; // Add padding byte
    }
    memcpy(output + index, s_bin, s_size);
    index += s_size;
}

Signature* Signature_parse(unsigned char* s) {
    unsigned char compound = s[0];
    if (compound != 0x30) {
        printf("Bad Signature\n");
        exit(1);
    }
    unsigned char length = s[1];
    unsigned char marker = s[2];
    if (marker != 0x02) {
        printf("Bad Signature\n");
        exit(1);
    }
    unsigned char r_length = s[3];
    unsigned char r_bin[r_length];
    memset(r_bin, 0, r_length);
    memcpy(r_bin, s + 4, r_length);
    marker = s[4 + r_length];
    if (marker != 0x02) {
        printf("Bad Signature\n");
        exit(1);
    }
    unsigned char s_length = s[5 + r_length];
    unsigned char s_bin[s_length];
    memset(s_bin, 0, s_length);
    memcpy(s_bin, s + 6 + r_length, s_length);
    if (length != r_length + s_length + 4) {
        printf("Signature too long\n");
        exit(1);
    }
    mpz_t r_mpz;
    mpz_init(r_mpz);
    mpz_import(r_mpz, r_length, 1, sizeof(r_bin[0]), 0, 0, r_bin);
    S256Field* r_field = S256Field_init(r_mpz);
    mpz_t s_mpz;
    mpz_init(s_mpz);
    mpz_import(s_mpz, s_length, 1, sizeof(s_bin[0]), 0, 0, s_bin);
    S256Field* s_field = S256Field_init(s_mpz);
    return Signature_init(r_field, s_field);
}