#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#include "helper.h"

const char b58digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

void sha256(const unsigned char *message, size_t message_len, unsigned char *digest) {
    EVP_MD_CTX *mdctx;
    
    if((mdctx = EVP_MD_CTX_new()) == NULL)
        return;
    
    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL))
        return;
    
    if(1 != EVP_DigestUpdate(mdctx, message, message_len))
        return;
    
    unsigned int digest_len;
    if(1 != EVP_DigestFinal_ex(mdctx, digest, &digest_len))
        return;
    
    EVP_MD_CTX_free(mdctx);
}

void ripemd160(const unsigned char *message, size_t message_len, unsigned char *digest) {
    RIPEMD160_CTX ctx;
    RIPEMD160_Init(&ctx);
    RIPEMD160_Update(&ctx, message, message_len);
    RIPEMD160_Final(digest, &ctx);
}

void hash160(const unsigned char *message, size_t message_len, unsigned char *digest) {
    unsigned char sha256_digest[32];
    sha256(message, message_len, sha256_digest);
    ripemd160(sha256_digest, 32, digest);
}

void hash256(const unsigned char *message, size_t message_len, unsigned char *digest) {
    sha256(message, message_len, digest);
    sha256(digest, 32, digest);
}

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

void memzero(void *const pnt, const size_t len) {
  volatile unsigned char *volatile pnt_ = (volatile unsigned char *volatile)pnt;
  size_t i = (size_t)0U;

  while (i < len) {
    pnt_[i++] = 0U;
  }
}

void encode_base58(unsigned char *b58, size_t *b58sz, const void *data, size_t binsz) {
  const uint8_t *bin = data;
  int carry;
  ssize_t i, j, high, zcount = 0;
  size_t size;

  while (zcount < (ssize_t)binsz && !bin[zcount]) ++zcount;

  size = (binsz - zcount) * 138 / 100 + 1;
  uint8_t buf[size];
  memzero(buf, size);

  for (i = zcount, high = size - 1; i < (ssize_t)binsz; ++i, high = j) {
    for (carry = bin[i], j = size - 1; (j > high) || carry; --j) {
      carry += 256 * buf[j];
      buf[j] = carry % 58;
      carry /= 58;
    }
  }

  for (j = 0; j < (ssize_t)size && !buf[j]; ++j)
    ;

  if (zcount) memset(b58, '1', zcount);
  for (i = zcount; j < (ssize_t)size; ++i, ++j) {
    b58[i] = b58digits_ordered[buf[j]];
  }
  b58[i] = '\0';
  *b58sz = i + 1;
}

void encode_base58_checksum(unsigned char *b58c, size_t *b58c_sz, const void *data, size_t binsz) {
  uint8_t buf[21 + 4] = {0};
  uint8_t hash[21] = {0};
  memcpy(buf, data, binsz);
  hash256(buf, binsz, hash);
  memcpy(buf + binsz, hash, 4);
  encode_base58(b58c, b58c_sz, buf, sizeof(buf));
}