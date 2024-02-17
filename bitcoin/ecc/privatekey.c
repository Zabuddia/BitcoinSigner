#include <string.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>

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

void mpz_to_bytes(const mpz_t op, unsigned char *out, size_t out_len) {
    size_t op_size = (mpz_sizeinbase(op, 2) + 7) / 8;
    mpz_export(out + (out_len - op_size), NULL, 1, 1, 0, 0, op);
}

void compute_hmac_sha256(unsigned char *key, int key_len,
                         unsigned char *data, int data_len,
                         unsigned char *output, size_t *output_len) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_PKEY *pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, key, key_len);

    EVP_DigestSignInit(ctx, NULL, EVP_sha256(), NULL, pkey);
    EVP_DigestSignUpdate(ctx, data, data_len);
    EVP_DigestSignFinal(ctx, output, output_len);

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);
}

S256Field* Deterministic_k(PrivateKey* key, S256Field* z) {
    unsigned char k[32] = {0};
    size_t k_len = 32;
    unsigned char v[32];
    memset(v, 0x01, 32);
    size_t v_len = 32;

    S256Field* zee = z;

    mpz_t n;
    mpz_init_set_str(n, N, 16);

    if (mpz_cmp(zee->num, n) > 0) {
        mpz_sub(zee->num, zee->num, n);
    }

    unsigned char z_bytes[32];
    unsigned char secret_bytes[32];
    mpz_to_bytes(zee->num, z_bytes, 32);
    mpz_to_bytes(key->e->num, secret_bytes, 32);

    //k = hmac.new(k, v + b'\x00' + secret_bytes + z_bytes, s256).digest()
    unsigned char data[sizeof(v) + 1 + sizeof(secret_bytes) + sizeof(z_bytes)];

    int offset = 0;
    memcpy(data + offset, v, sizeof(v));
    offset += sizeof(v);
    data[offset++] = 0x00;
    memcpy(data + offset, secret_bytes, sizeof(secret_bytes));
    offset += sizeof(secret_bytes);
    memcpy(data + offset, z_bytes, sizeof(z_bytes));

    compute_hmac_sha256(k, k_len - 1, data, sizeof(data) - 1, k, &k_len);

    //v = hmac.new(k, v, s256).digest()
    unsigned char data_v[sizeof(v)];

    memcpy(data_v, v, sizeof(v));

    compute_hmac_sha256(k, k_len - 1, data_v, sizeof(data_v) - 1, v, &v_len);

    //k = hmac.new(k, v + b'\x00' + secret_bytes + z_bytes, s256).digest()
    compute_hmac_sha256(k, k_len - 1, data, sizeof(data) - 1, k, &k_len);

    //v = hmac.new(k, v, s256).digest()
    compute_hmac_sha256(k, k_len - 1, data_v, sizeof(data_v) - 1, v, &v_len);

    // while True:
    // v = hmac.new(k, v, s256).digest()
    // candidate = int.from_bytes(v, 'big')
    // if candidate >= 1 and candidate < N:
    //     return candidate  # <2>
    // k = hmac.new(k, v + b'\x00', s256).digest()
    // v = hmac.new(k, v, s256).digest()

    mpz_t candidate;
    mpz_init(candidate);
    while (1) {
        //v = hmac.new(k, v, s256).digest()
        compute_hmac_sha256(k, k_len - 1, data_v, sizeof(data_v) - 1, v, &v_len);
        mpz_import(candidate, v_len, 1, sizeof(v[0]), 0, 0, v);
        if (mpz_cmp_ui(candidate, 1) >= 0 && mpz_cmp(candidate, n) < 0) {
            S256Field* K = S256Field_init(candidate);
            return K;
        }
        //k = hmac.new(k, v + b'\x00' + secret_bytes + z_bytes, s256).digest()
        compute_hmac_sha256(k, k_len - 1, data, sizeof(data) - 1, k, &k_len);
        //v = hmac.new(k, v, s256).digest()
        compute_hmac_sha256(k, k_len - 1, data_v, sizeof(data_v) - 1, v, &v_len);
    }

    //Will never get here
    mpz_t result;
    mpz_init(result);
    mpz_import(result, k_len, 1, sizeof(k[0]), 1, 0, k);

    S256Field* K = S256Field_init(result);

    return K;
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