#include "privatekey.h"

#define K_LEN 32
#define V_LEN 32
#define Z_LEN 32
#define SECRET_LEN 32
#define SHA256_DIGEST_LENGTH 32

#define WIF_RAW_COMPRESSED_LEN 34
#define WIF_COMPRESSED_LEN 52
#define WIF_RAW_UNCOMPRESSED_LEN 33
#define WIF_UNCOMPRESSED_LEN 51

PrivateKey* PrivateKey_init(mpz_t secret) {
    mpz_t gx;
    mpz_t gy;
    mpz_init_set_str(gx, GX, HEX);
    mpz_init_set_str(gy, GY, HEX);

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

void PrivateKey_free(PrivateKey* key) {
    if (key != NULL) {
        S256Field_free(key->e);
        S256Point_free(key->point);
        free(key);
    }
}

S256Field* Deterministic_k(PrivateKey* key, S256Field* z) {
    uint8_t k[K_LEN] = {0};
    uint8_t v[V_LEN];
    memset(v, 0x01, V_LEN);

    S256Field* zee = z;

    mpz_t n;
    mpz_init_set_str(n, N, HEX);

    if (mpz_cmp(zee->num, n) > 0) {
        mpz_sub(zee->num, zee->num, n);
    }

    uint8_t z_bytes[Z_LEN] = {0};
    uint8_t secret_bytes[SECRET_LEN] = {0};
    mpz_to_bytes(zee->num, z_bytes, Z_LEN);
    mpz_to_bytes(key->e->num, secret_bytes, SECRET_LEN);

    //k = hmac.new(k, v + b'\x00' + secret_bytes + z_bytes, s256).digest()
    uint8_t data_1[V_LEN + 1 + SECRET_LEN + Z_LEN];

    memcpy(data_1, v, V_LEN);
    data_1[V_LEN] = 0x00;
    memcpy(data_1 + V_LEN + 1, secret_bytes, SECRET_LEN);
    memcpy(data_1 + V_LEN + SECRET_LEN + 1, z_bytes, Z_LEN);

    uint32_t output_len = SHA256_DIGEST_LENGTH;
    compute_hmac_sha256(k, K_LEN, data_1, sizeof(data_1), k, &output_len);

    //v = hmac.new(k, v, s256).digest()
    uint8_t data_v[V_LEN];

    memcpy(data_v, v, V_LEN);

    compute_hmac_sha256(k, K_LEN, data_v, V_LEN, v, &output_len);

    //k = hmac.new(k, v + b'\x00' + secret_bytes + z_bytes, s256).digest()
    uint8_t data_2[V_LEN + 1 + SECRET_LEN + Z_LEN];

    memcpy(data_2, v, V_LEN);
    data_2[V_LEN] = 0x01;
    memcpy(data_2 + V_LEN + 1, secret_bytes, SECRET_LEN);
    memcpy(data_2 + V_LEN + SECRET_LEN + 1, z_bytes, Z_LEN);
    compute_hmac_sha256(k, K_LEN, data_2, sizeof(data_2), k, &output_len);

    //v = hmac.new(k, v, s256).digest()
    memcpy(data_v, v, V_LEN);
    compute_hmac_sha256(k, K_LEN, data_v, V_LEN, v, &output_len);

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
        memcpy(data_v, v, V_LEN);
        compute_hmac_sha256(k, K_LEN, data_v, V_LEN, v, &output_len);
        mpz_import(candidate, V_LEN, 1, sizeof(v[0]), 0, 0, v);
        if (mpz_cmp_ui(candidate, 1) >= 0 && mpz_cmp(candidate, n) < 0) {
            S256Field* K = S256Field_init(candidate);
            mpz_clear(n);
            return K;
        }
        //k = hmac.new(k, v + b'\x00' + secret_bytes + z_bytes, s256).digest()
        uint8_t data_3[V_LEN + 1];
        memcpy(data_3, v, V_LEN);
        data_3[V_LEN] = 0x00;
        compute_hmac_sha256(k, K_LEN, data_3, sizeof(data_3), k, &output_len);
        //v = hmac.new(k, v, s256).digest()
        compute_hmac_sha256(k, K_LEN, data_v, V_LEN, v, &output_len);
    }

    //Will never get here
    mpz_t result;
    mpz_init(result);
    mpz_import(result, K_LEN, 1, sizeof(k[0]), 1, 0, k);

    S256Field* K = S256Field_init(result);

    mpz_clear(n);

    return K;
}

Signature* PrivateKey_sign(PrivateKey* key, S256Field* z) {
    mpz_t gx;
    mpz_t gy;
    mpz_init_set_str(gx, GX, HEX);
    mpz_init_set_str(gy, GY, HEX);

    S256Field* x = S256Field_init(gx);
    S256Field* y = S256Field_init(gy);

    S256Point* G = S256Point_init(x, y);

    mpz_t n;
    mpz_init_set_str(n, N, HEX);

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
        S256Field_free(s);
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

void PrivateKey_wif(PrivateKey* key, uint8_t* output, bool compressed, bool testnet) {
    if (compressed) {
        uint8_t wif[WIF_RAW_COMPRESSED_LEN] = {0};
        if (testnet) {
            wif[0] = 0xef;
        } else {
            wif[0] = 0x80;
        }
        wif[33] = 0x01;
        mpz_to_bytes(key->e->num, wif + 1, 32);
        uint32_t output_len = WIF_COMPRESSED_LEN;
        encode_base58_checksum_wif_compressed(output, &output_len, wif, WIF_RAW_COMPRESSED_LEN);
    } else {
        uint8_t wif[WIF_RAW_UNCOMPRESSED_LEN] = {0};
        if (testnet) {
            wif[0] = 0xef;
        } else {
            wif[0] = 0x80;
        }
        mpz_to_bytes(key->e->num, wif + 1, 32);
        uint32_t output_len = WIF_UNCOMPRESSED_LEN;
        encode_base58_checksum_wif_uncompressed(output, &output_len, wif, WIF_RAW_UNCOMPRESSED_LEN);
    }
}