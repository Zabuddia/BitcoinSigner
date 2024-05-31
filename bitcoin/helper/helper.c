#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#include "helper.h"

const char b58digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

void sha1(const unsigned char* message, size_t message_len, unsigned char* digest) {
    EVP_MD_CTX *mdctx;
    
    if((mdctx = EVP_MD_CTX_new()) == NULL)
        return;
    
    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL))
        return;
    
    if(1 != EVP_DigestUpdate(mdctx, message, message_len))
        return;
    
    unsigned int digest_len;
    if(1 != EVP_DigestFinal_ex(mdctx, digest, &digest_len))
        return;
    
    EVP_MD_CTX_free(mdctx);
}

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
                  unsigned char *output, unsigned int *output_len) {
    if (!key || !data || !output || !output_len) {
        fprintf(stderr, "Invalid input to HMAC computation.\n");
        return;
    }

    HMAC_CTX *ctx;

    // Create and initialize HMAC context
    ctx = HMAC_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Failed to create HMAC context\n");
        return;
    }

    if (HMAC_Init_ex(ctx, key, key_len, EVP_sha256(), NULL) != 1) {
        fprintf(stderr, "Failed to initialize HMAC\n");
        HMAC_CTX_free(ctx);
        return;
    }

    // Provide input data
    if (HMAC_Update(ctx, data, data_len) != 1) {
        fprintf(stderr, "Failed to update HMAC\n");
        HMAC_CTX_free(ctx);
        return;
    }

    // Finalize the HMAC computation and fetch the result
    if (HMAC_Final(ctx, output, output_len) != 1) {
        fprintf(stderr, "Failed to finalize HMAC\n");
        HMAC_CTX_free(ctx);
        return;
    }

    // Clean up HMAC context
    HMAC_CTX_free(ctx);
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

void decode_base58(unsigned char* b58, size_t b58sz, unsigned char* out) {
    size_t size = 0;

    for (size_t i = 0; i < b58sz; i++) {
        unsigned char c = b58[i];
        int val = -1;
        if (c >= '1' && c <= '9') val = c - '1';
        else if (c >= 'A' && c <= 'H') val = c - 'A' + 9;
        else if (c >= 'J' && c <= 'N') val = c - 'J' + 17;
        else if (c >= 'P' && c <= 'Z') val = c - 'P' + 22;
        else if (c >= 'a' && c <= 'k') val = c - 'a' + 33;
        else if (c >= 'm' && c <= 'z') val = c - 'm' + 44;
        else {
            printf("Invalid character in base58 string\n");
            exit(1);
        }

        int carry = val;
        for (size_t j = 0; j <= size; j++) {
            carry += out[j] * 58;
            out[j] = carry % 256;
            carry /= 256;
        }

        while (carry) {
            out[++size] = carry % 256;
            carry /= 256;
        }
    }

    // Reverse the result
    for (size_t i = 0; i < size / 2; i++) {
        unsigned char temp = out[i];
        out[i] = out[size - i - 1];
        out[size - i - 1] = temp;
    }
}

void encode_base58_checksum_address(unsigned char *b58c, size_t *b58c_sz, const void *data, size_t binsz) {
    uint8_t buf[21 + 4] = {0};
    uint8_t hash[21] = {0};
    memcpy(buf, data, binsz);
    hash256(buf, binsz, hash);
    memcpy(buf + binsz, hash, 4);
    encode_base58(b58c, b58c_sz, buf, sizeof(buf));
}

void encode_base58_checksum_wif_uncompressed(unsigned char *b58c, size_t *b58c_sz, const void *data, size_t binsz) {
    uint8_t buf[33 + 4] = {0};
    uint8_t hash[33] = {0};
    memcpy(buf, data, binsz);
    hash256(buf, binsz, hash);
    memcpy(buf + binsz, hash, 4);
    encode_base58(b58c, b58c_sz, buf, sizeof(buf));
}

void encode_base58_checksum_wif_compressed(unsigned char *b58c, size_t *b58c_sz, const void *data, size_t binsz) {
    uint8_t buf[34 + 4] = {0};
    uint8_t hash[34] = {0};
    memcpy(buf, data, binsz);
    hash256(buf, binsz, hash);
    memcpy(buf + binsz, hash, 4);
    encode_base58(b58c, b58c_sz, buf, sizeof(buf));
}

int little_endian_to_int(const unsigned char *data, size_t data_len) {
    int result = 0;
    for (int i = 0; i < data_len; i++) {
        result += data[i] << (8 * i);
    }
    return result;
}

unsigned long long little_endian_to_long(const unsigned char *data, size_t data_len) {
    unsigned long long result = 0;
    for (int i = 0; i < data_len; i++) {
        result += (unsigned long long)data[i] << (8 * i);
    }
    return result;
}

void int_to_little_endian(int num, unsigned char *output, size_t output_len) {
    for (int i = 0; i < output_len; i++) {
        output[i] = (num >> (8 * i)) & 0xff;
    }
}

void long_to_little_endian(unsigned long long num, unsigned char *output, size_t output_len) {
    for (int i = 0; i < output_len; i++) {
        output[i] = (num >> (8 * i)) & 0xff;
    }
}

void print_formatted_bytes(const unsigned char* hex) {
    int len = strlen((const char*)hex);
    int i;
    printf("Formatted output:\n");
    for (i = 0; i < len; i += 2) {
        unsigned int byte;
        sscanf((const char*)hex + i, "%2x", &byte);
        printf("0x%02x, ", byte);
    }
    printf("\n");
    printf("Length: %d\n", i / 2);
}

void find_differences(const char* data1, const char* data2) {
    int data_len = strlen(data1);
    if (data_len != strlen(data2)) {
        printf("Data lengths are different\n");
        return;
    }
    for (int i = 0; i < data_len; i++) {
        if (data1[i] != data2[i]) {
            printf("Difference found at index %d\n", i);
            printf("Data1: %d\n", data1[i]);
            printf("Data2: %d\n", data2[i]);
        }
    }
}

unsigned long long read_varint(unsigned char* data) {
    unsigned char i = data[0];
    if (i == 0xfd) {
        return (unsigned long long)little_endian_to_int(data + 1, 2);
    } else if (i == 0xfe) {
        return (unsigned long long)little_endian_to_long(data + 1, 4);
    } else if (i == 0xff) {
        return little_endian_to_long(data + 1, 8);
    } else {
        return (unsigned long long)i;
    }
}

void encode_varint(unsigned char* output, unsigned long long num) {
    if (num < 0xfd) {
        output[0] = num;
    } else if (num <= 0xffff) {
        output[0] = 0xfd;
        int_to_little_endian(num, output + 1, 2);
    } else if (num <= 0xffffffff) {
        output[0] = 0xfe;
        long_to_little_endian(num, output + 1, 4);
    } else {
        output[0] = 0xff;
        long_to_little_endian(num, output + 1, 8);
    }
}

void little_endian_to_big_endian(unsigned char* data, size_t data_len) {
    unsigned char* temp = (unsigned char*)malloc(data_len);
    if (temp == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < data_len; i++) {
        temp[i] = data[data_len - i - 1];
    }
    memcpy(data, temp, data_len);
    free(temp);
}

unsigned char hex_char_to_byte(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    } else if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }
    return 0; // Not a hex char
}

void hex_string_to_byte_array(const char* hexStr, unsigned char* byteArray) {
    size_t len = strlen(hexStr);
    if (len % 2 != 0) {
        printf("Hex string must have even length\n");
        exit(1);
    }

    size_t nBytes = len / 2;

    for (size_t i = 0; i < nBytes; i++) {
        unsigned char high = hex_char_to_byte(hexStr[2 * i]);
        unsigned char low = hex_char_to_byte(hexStr[2 * i + 1]);
        byteArray[i] = (high << 4) | low;
    }
}

void byte_array_to_hex_string(unsigned char* byte_array, size_t byte_array_len, char* hex_string) {
    for (size_t i = 0; i < byte_array_len; i++) {
        sprintf(hex_string + 2 * i, "%02x", byte_array[i]);
    }
    hex_string[2 * byte_array_len] = '\0';
}

void h160_to_p2pkh_address(unsigned char* h160, unsigned char* address, size_t* address_size, __uint8_t testnet) {
    unsigned char prefix_plus_h160[21] = {0};
    if (testnet) {
        prefix_plus_h160[0] = 0x6f;
    } else {
        prefix_plus_h160[0] = 0x00;
    }
    memcpy(prefix_plus_h160 + 1, h160, 20);
    encode_base58_checksum_address(address, address_size, prefix_plus_h160, 21);
}

void h160_to_p2sh_address(unsigned char* h160, unsigned char* address, size_t* address_size, __uint8_t testnet) {
    unsigned char prefix_plus_h160[21] = {0};
    if (testnet) {
        prefix_plus_h160[0] = 0xc4;
    } else {
        prefix_plus_h160[0] = 0x05;
    }
    memcpy(prefix_plus_h160 + 1, h160, 20);
    encode_base58_checksum_address(address, address_size, prefix_plus_h160, 21);
}