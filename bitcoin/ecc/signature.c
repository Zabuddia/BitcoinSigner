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

void Signature_der(Signature* sig, unsigned char* output) {
    output[0] = 0x30;

    size_t total_bytes;

    size_t place;
    
    void *ptr = malloc(100);
    size_t count_r;
    mpz_export(ptr, &count_r, 1, sizeof(mp_limb_t), 0, 0, sig->r->num);
    size_t num_bytes_r = count_r * sizeof(mp_limb_t);

    size_t count_s;
    mpz_export(ptr, &count_s, 1, sizeof(mp_limb_t), 0, 0, sig->s->num);
    size_t num_bytes_s = count_s * sizeof(mp_limb_t);

    total_bytes = 4 + num_bytes_r + num_bytes_s;

    output[2] = 0x02;

    unsigned char* r_bin = malloc(num_bytes_r);
    if (r_bin == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    mpz_export(r_bin, NULL, 1, 1, 0, 0, sig->r->num);

    if (r_bin[0] >= 0x80) {
        total_bytes += 1;
        output[3] = num_bytes_r + 1;
        output[4] = 0x00;
        mpz_to_bytes(sig->r->num, output + 5, num_bytes_r);
        place = 5 + num_bytes_r;
    } else {
        output[3] = num_bytes_r;
        mpz_to_bytes(sig->r->num, output + 4, num_bytes_r);
        place = 4 + num_bytes_r;
    }

    output[place] = 0x02;

    unsigned char* s_bin = malloc(num_bytes_s);
    if (s_bin == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    mpz_export(s_bin, NULL, 1, 1, 0, 0, sig->s->num);

    if (s_bin[0] >= 0x80) {
        total_bytes += 1;
        output[place + 1] = num_bytes_s + 1;
        output[place + 2] = 0x00;
        mpz_to_bytes(sig->s->num, output + place + 3, num_bytes_s);
    } else {
        output[place + 1] = num_bytes_s;
        mpz_to_bytes(sig->s->num, output + place + 2, num_bytes_s);
    }

    output[1] = total_bytes;

    free(r_bin);
    free(s_bin);
    free(ptr);
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