#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "helper.h"

const char b58digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

void sha1(const uint8_t* message, uint32_t message_len, uint8_t* digest) {
    EVP_MD_CTX* mdctx;
    
    if((mdctx = EVP_MD_CTX_new()) == NULL)
        return;
    
    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL))
        return;
    
    if(1 != EVP_DigestUpdate(mdctx, message, message_len))
        return;
    
    uint32_t digest_len;
    if(1 != EVP_DigestFinal_ex(mdctx, digest, &digest_len))
        return;
    
    EVP_MD_CTX_free(mdctx);
}

void sha256(const uint8_t* message, uint32_t message_len, uint8_t* digest) {
    EVP_MD_CTX* mdctx;
    
    if((mdctx = EVP_MD_CTX_new()) == NULL)
        return;
    
    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL))
        return;
    
    if(1 != EVP_DigestUpdate(mdctx, message, message_len))
        return;
    
    uint32_t digest_len;
    if(1 != EVP_DigestFinal_ex(mdctx, digest, &digest_len))
        return;
    
    EVP_MD_CTX_free(mdctx);
}

void ripemd160(const uint8_t* message, uint32_t message_len, uint8_t* digest) {
    RIPEMD160_CTX ctx;
    RIPEMD160_Init(&ctx);
    RIPEMD160_Update(&ctx, message, message_len);
    RIPEMD160_Final(digest, &ctx);
}

void hash160(const uint8_t* message, uint32_t message_len, uint8_t* digest) {
    uint8_t sha256_digest[HASH256_LEN];
    sha256(message, message_len, sha256_digest);
    ripemd160(sha256_digest, HASH256_LEN, digest);
}

void hash256(const uint8_t* message, uint32_t message_len, uint8_t* digest) {
    sha256(message, message_len, digest);
    sha256(digest, HASH256_LEN, digest);
}

void hash_to_mpz_t(const uint8_t* data, uint32_t data_len, mpz_t res) {
    mpz_init(res);

    uint8_t hash[HASH256_LEN];
    hash256(data, data_len, hash);
    // gmp_printf("Hash: %Zx\n", hash);

    mpz_import(res, HASH256_LEN, 1, sizeof(hash[0]), 1, 0, hash);
    little_endian_to_big_endian((uint8_t*)res->_mp_d, res->_mp_size * sizeof(mp_limb_t));
    //gmp_printf("Hash to mpz_t: %Zx\n", res);
}

void mpz_to_bytes(const mpz_t op, uint8_t *out, uint32_t out_len) {
    uint32_t op_size = (mpz_sizeinbase(op, 2) + 7) / 8;
    mpz_export(out + (out_len - op_size), NULL, 1, 1, 0, 0, op);
}

void mpz_to_32bytes(mpz_t num, uint8_t *output) {
    size_t count = 0;
    mpz_export(output, &count, 1, 1, 1, 0, num);
    if (count < 32) {
        //If the number takes up less than 32 bytes, move the bytes to the end and prepend zeros
        memmove(output + (32 - count), output, count);
        memset(output, 0, 32 - count);
    }
}

void compute_hmac_sha256(uint8_t *key, int32_t key_len,
                  uint8_t *data, int32_t data_len,
                  uint8_t *output, uint32_t *output_len) {
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

void memzero(void* const pnt, const uint32_t len) {
    volatile uint8_t *volatile pnt_ = (volatile uint8_t *volatile)pnt;
    uint32_t i = (uint32_t)0U;

    while (i < len) {
    pnt_[i++] = 0U;
    }
}

void encode_base58(uint8_t* b58, uint32_t* b58sz, const void* data, uint32_t binsz) {
    if (!data || !b58 || !b58sz) {
        fprintf(stderr, "Null pointer input\n");
        return;
    }

    const uint8_t* bin = data;
    int32_t carry;
    int32_t i, j, high, zcount = 0;
    uint32_t size;

    // Count leading zeroes
    while (zcount < binsz && !bin[zcount]) ++zcount;

    // Calculate buffer size
    size = (binsz - zcount) * 138 / 100 + 1;
    uint8_t* buf = malloc(size);
    if (!buf) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    memzero(buf, size);

    // Perform the base58 encoding
    for (i = zcount, high = size - 1; i < binsz; ++i) {
        carry = bin[i];
        for (j = size - 1; (j > high) || carry; --j) {
            if (j >= size) {
                fprintf(stderr, "Index out of bounds: j=%u, size=%u\n", j, size);
                free(buf);
                return;
            }
            carry += 256 * buf[j];
            buf[j] = carry % 58;
            carry /= 58;
        }
        high = j;
    }

    // Skip leading zeroes in buffer
    for (j = 0; j < size && !buf[j]; ++j);

    // Output leading '1' characters for leading zeroes in input
    if (zcount) memset(b58, '1', zcount);
    // Output base58 digits
    for (i = zcount; j < size; ++i, ++j) {
        b58[i] = b58digits_ordered[buf[j]];
    }
    b58[i] = '\0';
    *b58sz = i + 1;

    free(buf);
}

int32_t base58_index(char c) {
    for (int32_t i = 0; i < 58; i++) {
        if (b58digits_ordered[i] == c) {
            return i;
        }
    }
    return -1; // Character not found
}

void decode_base58(const char* s, unsigned char* result) {
    mpz_t num;
    mpz_init(num);
    mpz_set_ui(num, 0);
    for (int32_t i = 0; i < strlen(s); i++) {
        int32_t index = base58_index(s[i]);
        if (index == -1) {
            fprintf(stderr, "Invalid character in base58 string\n");
            exit(1);
        }
        mpz_mul_ui(num, num, 58);
        mpz_add_ui(num, num, index);
    }

    unsigned char combined[25];
    
    mpz_to_bytes(num, combined, 25);

    memcpy(result, combined + 1, 20);

    mpz_clear(num);
}

void encode_base58_checksum_address(uint8_t *b58c, uint32_t *b58c_sz, const void *data, uint32_t binsz) {
    uint8_t buf[21 + 4] = {0};
    uint8_t hash[21] = {0};
    memcpy(buf, data, binsz);
    // I don't know why I have to do the next two lines. That is the way to get it to work on the Raspberry Pi. 
    uint8_t buf_copy[25] = {0};
    memcpy(buf_copy, buf, 21);
    hash256(buf, binsz, hash);
    memcpy(buf_copy + binsz, hash, 4);
    encode_base58(b58c, b58c_sz, buf_copy, sizeof(buf_copy));
}

void encode_base58_checksum_wif_uncompressed(uint8_t *b58c, uint32_t *b58c_sz, const void *data, uint32_t binsz) {
    uint8_t buf[33 + 4] = {0};
    uint8_t hash[33] = {0};
    memcpy(buf, data, binsz);
    hash256(buf, binsz, hash);
    memcpy(buf + binsz, hash, 4);
    encode_base58(b58c, b58c_sz, buf, sizeof(buf));
}

void encode_base58_checksum_wif_compressed(uint8_t *b58c, uint32_t *b58c_sz, const void *data, uint32_t binsz) {
    uint8_t buf[34 + 4] = {0};
    uint8_t hash[34] = {0};
    memcpy(buf, data, binsz);
    hash256(buf, binsz, hash);
    memcpy(buf + binsz, hash, 4);
    encode_base58(b58c, b58c_sz, buf, sizeof(buf));
}

int32_t little_endian_to_int(const uint8_t *data, uint32_t data_len) {
    int32_t result = 0;
    for (int i = 0; i < data_len; i++) {
        result += data[i] << (8 * i);
    }
    return result;
}

uint64_t little_endian_to_long(const uint8_t *data, uint32_t data_len) {
    uint64_t result = 0;
    for (int i = 0; i < data_len; i++) {
        result += (uint64_t)data[i] << (8 * i);
    }
    return result;
}

void int_to_little_endian(int32_t num, uint8_t *output, uint32_t output_len) {
    for (int32_t i = 0; i < output_len; i++) {
        output[i] = (num >> (8 * i)) & 0xff;
    }
}

void long_to_little_endian(uint64_t num, uint8_t *output, uint32_t output_len) {
    for (int32_t i = 0; i < output_len; i++) {
        output[i] = (num >> (8 * i)) & 0xff;
    }
}

void print_formatted_bytes(const uint8_t* hex) {
    int32_t len = strlen((const char*)hex);
    int32_t i;
    printf("Formatted output:\n");
    for (i = 0; i < len; i += 2) {
        uint32_t byte;
        sscanf((const char*)hex + i, "%2x", &byte);
        printf("0x%02x, ", byte);
    }
    printf("\n");
    printf("Length: %d\n", i / 2);
}

void find_differences(const char* data1, const char* data2) {
    int32_t data_len = strlen(data1);
    if (data_len != strlen(data2)) {
        printf("Data lengths are different\n");
        return;
    }
    for (int32_t i = 0; i < data_len; i++) {
        if (data1[i] != data2[i]) {
            printf("Difference found at index %d\n", i);
            printf("Data1: %d\n", data1[i]);
            printf("Data2: %d\n", data2[i]);
        }
    }
}

uint64_t read_varint(uint8_t* data) {
    uint8_t i = data[0];
    if (i == 0xfd) {
        return (uint64_t)little_endian_to_int(data + 1, 2);
    } else if (i == 0xfe) {
        return (uint64_t)little_endian_to_long(data + 1, 4);
    } else if (i == 0xff) {
        return little_endian_to_long(data + 1, 8);
    } else {
        return (uint64_t)i;
    }
}

uint8_t read_varint_size(uint8_t* data) {
    uint8_t i = data[0];
    if (i == 0xfd) {
        return 3;
    } else if (i == 0xfe) {
        return 5;
    } else if (i == 0xff) {
        return 9;
    } else {
        return 1;
    }
}

uint8_t add_varint_size(uint64_t num) {
    if (num < 0xfd) {
        return 1;
    } else if (num <= 0xffff) {
        return 3;
    } else if (num <= 0xffffffff) {
        return 5;
    } else {
        return 9;
    }
}

void encode_varint(uint8_t* output, uint64_t num) {
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

void little_endian_to_big_endian(uint8_t* data, uint32_t data_len) {
    uint8_t* temp = (uint8_t*)malloc(data_len);
    if (temp == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    for (int32_t i = 0; i < data_len; i++) {
        temp[i] = data[data_len - i - 1];
    }
    memcpy(data, temp, data_len);
    free(temp);
}

uint8_t hex_char_to_byte(uint8_t c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    } else if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }
    return 0; // Not a hex char
}

void hex_string_to_byte_array(const char* hexStr, uint8_t* byteArray) {
    uint32_t len = strlen(hexStr);
    if (len % 2 != 0) {
        printf("Hex string must have even length\n");
        exit(1);
    }

    uint32_t nBytes = len / 2;

    for (uint32_t i = 0; i < nBytes; i++) {
        uint8_t high = hex_char_to_byte(hexStr[2 * i]);
        uint8_t low = hex_char_to_byte(hexStr[2 * i + 1]);
        byteArray[i] = (high << 4) | low;
    }
}

void byte_array_to_hex_string(uint8_t* byte_array, uint32_t byte_array_len, char* hex_string) {
    for (uint32_t i = 0; i < byte_array_len; i++) {
        sprintf(hex_string + 2 * i, "%02x", byte_array[i]);
    }
    hex_string[2 * byte_array_len] = '\0';
}

void h160_to_p2pkh_address(uint8_t* h160, uint8_t* address, uint32_t* address_size, bool testnet) {
    uint8_t prefix_plus_h160[21] = {0};
    if (testnet) {
        prefix_plus_h160[0] = 0x6f;
    } else {
        prefix_plus_h160[0] = 0x00;
    }
    memcpy(prefix_plus_h160 + 1, h160, 20);
    encode_base58_checksum_address(address, address_size, prefix_plus_h160, 21);
}

void h160_to_p2sh_address(uint8_t* h160, uint8_t* address, uint32_t* address_size, bool testnet) {
    uint8_t prefix_plus_h160[21] = {0};
    if (testnet) {
        prefix_plus_h160[0] = 0xc4;
    } else {
        prefix_plus_h160[0] = 0x05;
    }
    memcpy(prefix_plus_h160 + 1, h160, 20);
    encode_base58_checksum_address(address, address_size, prefix_plus_h160, 21);
}