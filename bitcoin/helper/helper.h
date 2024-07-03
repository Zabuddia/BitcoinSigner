#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <gmp.h>
#include <openssl/evp.h>
#include <openssl/ripemd.h>
#include <openssl/hmac.h>
#include <sys/types.h>

#define HEX 16

#define HASH256_LEN 32

#define SIGHASH_ALL 1

#define P "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"
#define N "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141"
#define GX "79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"
#define GY "483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"

extern mpz_t PRIME;
extern mpz_t A;
extern mpz_t B;
extern mpz_t gx;
extern mpz_t gy;

void sha1(const uint8_t* message, uint32_t message_len, uint8_t* digest);

void sha256(const uint8_t *message, uint32_t message_len, uint8_t *digest);

void ripemd160(const uint8_t *message, uint32_t message_len, uint8_t *digest);

void hash160(const uint8_t *message, uint32_t message_len, uint8_t *digest);

void hash256(const uint8_t *message, uint32_t message_len, uint8_t *digest);

void hash_to_mpz_t(const uint8_t* data, uint32_t data_len, mpz_t res);

void mpz_to_bytes(const mpz_t op, uint8_t *out, uint32_t out_len);

void mpz_to_32bytes(mpz_t num, uint8_t *output);

void compute_hmac_sha256(uint8_t *key, int32_t key_len,
                  uint8_t *data, int32_t data_len,
                  uint8_t *output, uint32_t* output_len);

void memzero(void* const pnt, const uint32_t len);

void encode_base58(uint8_t *b58, uint32_t *b58sz, const void *data, uint32_t binsz);

int32_t base58_index(char c);

void decode_base58(const char* s, unsigned char* result);

// void decode_base58(uint8_t* b58, uint32_t b58sz, uint8_t* out, uint32_t outsz);

void encode_base58_checksum_address(uint8_t *b58c, uint32_t *b58c_sz, const void *data, uint32_t binsz);

void encode_base58_checksum_wif_uncompressed(uint8_t *b58c, uint32_t *b58c_sz, const void *data, uint32_t binsz);

void encode_base58_checksum_wif_compressed(uint8_t *b58c, uint32_t *b58c_sz, const void *data, uint32_t binsz);

//Max data_len is 4
int32_t little_endian_to_int(const uint8_t *data, uint32_t data_len);

//Max data_len is 8
uint64_t little_endian_to_long(const uint8_t *data, uint32_t data_len);

//Max output_len is 4
void int_to_little_endian(int32_t num, uint8_t *output, uint32_t output_len);

//Max output_len is 8
void long_to_little_endian(uint64_t num, uint8_t *output, uint32_t output_len);

void print_formatted_bytes(const uint8_t* hex);

void find_differences(const char* data1, const char* data2);

uint64_t read_varint(uint8_t* data);

uint8_t read_varint_size(uint8_t* data);

uint8_t add_varint_size(uint64_t num);

void encode_varint(uint8_t* output, uint64_t num);

void little_endian_to_big_endian(uint8_t* data, uint32_t data_len);

uint8_t hex_char_to_byte(uint8_t c);

void hex_string_to_byte_array(const char* hexStr, uint8_t* byteArray);

void byte_array_to_hex_string(uint8_t* byte_array, uint32_t byte_array_len, char* hex_string);

void h160_to_p2pkh_address(uint8_t* h160, uint8_t* address, uint32_t* address_size, bool testnet);

void h160_to_p2sh_address(uint8_t* h160, uint8_t* address, uint32_t* address_size, bool testnet);

#endif //HELPER_H