#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#include "helper.h"

void hash_to_mpz_t(const unsigned char* data, size_t data_len, mpz_t res) {
    mpz_init(res);

    EVP_MD_CTX *mdctx;
    unsigned char hash[EVP_MAX_MD_SIZE]; // Buffer for hash output
    unsigned int hash_len;

    // Initialize context for the first round of SHA-256
    if((mdctx = EVP_MD_CTX_new()) == NULL) {
        perror("EVP_MD_CTX_new failed");
        exit(1);
    }

    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)) {
        perror("EVP_DigestInit_ex failed");
        EVP_MD_CTX_free(mdctx);
        exit(1);
    }

    if(1 != EVP_DigestUpdate(mdctx, data, data_len)) {
        perror("EVP_DigestUpdate failed");
        EVP_MD_CTX_free(mdctx);
        exit(1);
    }

    if(1 != EVP_DigestFinal_ex(mdctx, hash, &hash_len)) {
        perror("EVP_DigestFinal_ex failed");
        EVP_MD_CTX_free(mdctx);
        exit(1);
    }

    // Re-initialize context for the second round of SHA-256 using the hash from the first round
    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)) {
        perror("EVP_DigestInit_ex failed");
        EVP_MD_CTX_free(mdctx);
        exit(1);
    }

    if(1 != EVP_DigestUpdate(mdctx, hash, hash_len)) {
        perror("EVP_DigestUpdate failed");
        EVP_MD_CTX_free(mdctx);
        exit(1);
    }

    // Finalize the second round of hashing
    if(1 != EVP_DigestFinal_ex(mdctx, hash, &hash_len)) {
        perror("EVP_DigestFinal_ex failed");
        EVP_MD_CTX_free(mdctx);
        exit(1);
    }

    EVP_MD_CTX_free(mdctx);

    mpz_import(res, hash_len, 1, sizeof(hash[0]), 0, 0, hash);

    //S256Field* result = S256Field_init(res);
}

void mpz_to_bytes(const mpz_t op, unsigned char *out, size_t out_len) {
    size_t op_size = (mpz_sizeinbase(op, 2) + 7) / 8;
    mpz_export(out + (out_len - op_size), NULL, 1, 1, 0, 0, op);
}

void mpz_to_32bytes(mpz_t num, unsigned char *output) {
    size_t count = 0;
    mpz_export(output, &count, 1, 1, 1, 0, num);
    if (count < 32) {
        //If the number takes up less than 32 bytes, move the bytes to the end and prepend zeros
        memmove(output + (32 - count), output, count);
        memset(output, 0, 32 - count);
    }
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