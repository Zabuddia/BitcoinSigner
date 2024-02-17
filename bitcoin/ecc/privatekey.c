#include "privatekey.h"

PrivateKey* PrivateKey_init(const char* secret) {
    mpz_t gx;
    mpz_t gy;
    mpz_init_set_str(gx, GX, 16);
    mpz_init_set_str(gy, GY, 16);

    S256Field* x = S256Field_init(gx);
    S256Field* y = S256Field_init(gy);

    S256Point* G = S256Point_init(x, y);

    S256Field* e = hash_to_s256field((const unsigned char*)secret, strlen(secret));

    PrivateKey* key = malloc(sizeof(PrivateKey));
    key->secret = secret;
    key->e = e;
    key->point = S256Point_mul(G, e->num);
    return key;
}

Signature* PrivateKey_sign(PrivateKey* key, S256Field* z) {
    mpz_t gx;
    mpz_t gy;
    mpz_init_set_str(gx, GX, 16);
    mpz_init_set_str(gy, GY, 16);

    S256Field* x = S256Field_init(gx);
    S256Field* y = S256Field_init(gy);

    S256Point* G = S256Point_init(x, y);

    mpz_t n;
    mpz_init_set_str(n, N, 16);

    mpz_t kay;
    mpz_init_set_ui(kay, 1234567890);
    S256Field* k = S256Field_init(kay);

    S256Field* r = S256Point_mul(G, k->num)->x;

    S256Field* k_inv = S256Field_s_inv(k);

    S256Field* r_times_e = S256Field_s_mul(r, key->e);
    S256Field* z_plus_r_times_e = S256Field_add(z, r_times_e);
    S256Field* s = S256Field_s_mul(z_plus_r_times_e, k_inv);

    mpz_t n_dividedby_two;
    mpz_init(n_dividedby_two);
    mpz_div_ui(n_dividedby_two, n, 2);
    if (mpz_cmp(s->num, n_dividedby_two) > 0) {
        S256Field* en = S256Field_init(n);
        S256Field* n_minus_s = S256Field_sub(en, s);
        Signature* sig = Signature_init(r, n_minus_s);
        return sig;
    } else {
        Signature* sig = Signature_init(r, s);
        return sig;
    }
}