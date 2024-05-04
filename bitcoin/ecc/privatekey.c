#include <string.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>

#include "privatekey.h"

PrivateKey* PrivateKey_init(mpz_t secret) {
    mpz_t gx;
    mpz_t gy;
    mpz_init_set_str(gx, GX, 16);
    mpz_init_set_str(gy, GY, 16);

    S256Field* x = S256Field_init(gx);
    S256Field* y = S256Field_init(gy);

    S256Point* G = S256Point_init(x, y);

    S256Field* e = S256Field_init(secret);

    PrivateKey* key = malloc(sizeof(PrivateKey));
    key->e = e;
    key->point = S256Point_mul(G, e->num);
    S256Point_free(G);
    return key;
}

// PrivateKey* PrivateKey_init(const char* secret) {
//     mpz_t gx;
//     mpz_t gy;
//     mpz_init_set_str(gx, GX, 16);
//     mpz_init_set_str(gy, GY, 16);

//     S256Field* x = S256Field_init(gx);
//     S256Field* y = S256Field_init(gy);

//     S256Point* G = S256Point_init(x, y);

//     mpz_t E;
//     hash_to_mpz_t((const unsigned char*)secret, strlen(secret), E);

//     S256Field* e = S256Field_init(E);

//     PrivateKey* key = malloc(sizeof(PrivateKey));
//     key->secret = secret;
//     key->e = e;
//     key->point = S256Point_mul(G, e->num);
//     return key;
// }

void PrivateKey_free(PrivateKey* key) {
    if (key != NULL) {
        S256Field_free(key->e);
        S256Point_free(key->point);
        free(key);
    }
}

S256Field* Deterministic_k(PrivateKey* key, S256Field* z) {
    unsigned char k[32] = {0};
    int k_len = 32;
    unsigned char v[32];
    memset(v, 0x01, 32);
    int v_len = 32;

    S256Field* zee = z;

    mpz_t n;
    mpz_init_set_str(n, N, 16);

    if (mpz_cmp(zee->num, n) > 0) {
        mpz_sub(zee->num, zee->num, n);
    }

    unsigned char z_bytes[32] = {0};
    unsigned char secret_bytes[32] = {0};
    mpz_to_bytes(zee->num, z_bytes, 32);
    mpz_to_bytes(key->e->num, secret_bytes, 32);

    //k = hmac.new(k, v + b'\x00' + secret_bytes + z_bytes, s256).digest()
    unsigned char data_1[sizeof(v) + 1 + sizeof(secret_bytes) + sizeof(z_bytes)];

    memcpy(data_1, v, sizeof(v));
    data_1[sizeof(v)] = 0x00;
    memcpy(data_1 + sizeof(v) + 1, secret_bytes, sizeof(secret_bytes));
    memcpy(data_1 + sizeof(v) + sizeof(secret_bytes) + 1, z_bytes, sizeof(z_bytes));

    unsigned int output_len = 32;
    compute_hmac_sha256(k, k_len, data_1, sizeof(data_1), k, &output_len);

    //v = hmac.new(k, v, s256).digest()
    unsigned char data_v[sizeof(v)];

    memcpy(data_v, v, sizeof(v));

    compute_hmac_sha256(k, k_len, data_v, sizeof(data_v), v, &output_len);

    //k = hmac.new(k, v + b'\x00' + secret_bytes + z_bytes, s256).digest()
    unsigned char data_2[sizeof(v) + 1 + sizeof(secret_bytes) + sizeof(z_bytes)];

    memcpy(data_2, v, sizeof(v));
    data_2[sizeof(v)] = 0x01;
    memcpy(data_2 + sizeof(v) + 1, secret_bytes, sizeof(secret_bytes));
    memcpy(data_2 + sizeof(v) + sizeof(secret_bytes) + 1, z_bytes, sizeof(z_bytes));
    compute_hmac_sha256(k, k_len, data_2, sizeof(data_2), k, &output_len);

    //v = hmac.new(k, v, s256).digest()
    memcpy(data_v, v, sizeof(v));
    compute_hmac_sha256(k, k_len, data_v, sizeof(data_v), v, &output_len);

    // while True:
    // v = hmac.new(k, v, s256).digest()
    // candidate = int.from_bytes(v, 'big')
    // if candidate >= 1 and candidate < N:
    //     return candidate
    // k = hmac.new(k, v + b'\x00', s256).digest()
    // v = hmac.new(k, v, s256).digest()

    mpz_t candidate;
    mpz_init(candidate);
    while (1) {
        //v = hmac.new(k, v, s256).digest()
        memcpy(data_v, v, sizeof(v));
        compute_hmac_sha256(k, k_len, data_v, sizeof(data_v), v, &output_len);
        mpz_import(candidate, v_len, 1, sizeof(v[0]), 0, 0, v);
        if (mpz_cmp_ui(candidate, 1) >= 0 && mpz_cmp(candidate, n) < 0) {
            S256Field* K = S256Field_init(candidate);
            mpz_clear(n);
            return K;
        }
        //k = hmac.new(k, v + b'\x00' + secret_bytes + z_bytes, s256).digest()
        unsigned char data_3[sizeof(v) + 1];
        memcpy(data_3, v, sizeof(v));
        data_3[sizeof(v)] = 0x00;
        compute_hmac_sha256(k, k_len, data_3, sizeof(data_3), k, &output_len);
        //v = hmac.new(k, v, s256).digest()
        compute_hmac_sha256(k, k_len, data_v, sizeof(data_v), v, &output_len);
    }

    //Will never get here
    mpz_t result;
    mpz_init(result);
    mpz_import(result, k_len, 1, sizeof(k[0]), 1, 0, k);

    S256Field* K = S256Field_init(result);

    mpz_clear(n);

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

    S256Field* k = Deterministic_k(key, z);

    S256Field* r = S256Point_mul(G, k->num)->x;

    S256Field* k_inv = S256Field_s_inv(k);
    
    mpz_t r_times_e;
    mpz_init(r_times_e);
    mpz_mul(r_times_e, r->num, key->e->num);

    mpz_t z_plus_r_times_e;
    mpz_init(z_plus_r_times_e);
    mpz_add(z_plus_r_times_e, z->num, r_times_e);

    S256Field* s = S256Field_s_mul_scalar(k_inv, z_plus_r_times_e);

    mpz_clear(r_times_e);
    mpz_clear(z_plus_r_times_e);

    mpz_t n_dividedby_two;
    mpz_init(n_dividedby_two);
    mpz_div_ui(n_dividedby_two, n, 2);
    if (mpz_cmp(s->num, n_dividedby_two) > 0) {
        S256Field* en = S256Field_init(n);
        S256Field* n_minus_s = S256Field_sub(en, s);
        Signature* sig = Signature_init(r, n_minus_s);
        S256Field_free(k);
        S256Field_free(k_inv);
        S256Field_free(en);
        S256Point_free(G);
        mpz_clear(n);
        mpz_clear(n_dividedby_two);
        return sig;
    } else {
        Signature* sig = Signature_init(r, s);
        S256Field_free(k);
        S256Field_free(k_inv);
        S256Point_free(G);
        mpz_clear(n);
        mpz_clear(n_dividedby_two);
        return sig;
    }
}

void PrivateKey_wif(PrivateKey* key, unsigned char* output, uint8_t compressed, uint8_t testnet) {
    if (compressed) {
        unsigned char wif[34] = {0};
        if (testnet) {
            wif[0] = 0xef;
        } else {
            wif[0] = 0x80;
        }
        wif[33] = 0x01;
        mpz_to_bytes(key->e->num, wif + 1, 32);
        size_t output_len = 52;
        encode_base58_checksum_wif_compressed(output, &output_len, wif, 34);
    } else {
        unsigned char wif[33] = {0};
        if (testnet) {
            wif[0] = 0xef;
        } else {
            wif[0] = 0x80;
        }
        mpz_to_bytes(key->e->num, wif + 1, 32);
        size_t output_len = 52;
        encode_base58_checksum_wif_uncompressed(output, &output_len, wif, 33);
    }
}