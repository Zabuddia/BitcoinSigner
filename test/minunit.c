#include <stdio.h>

#include "minunit.h"

#include "../bitcoin/ecc/s256field.h"
#include "../bitcoin/ecc/s256point.h"
#include "../bitcoin/ecc/signature.h"
#include "../bitcoin/ecc/privatekey.h"

//For testing compressed sec and adding points with the same x
#define TEST_N "6d183de4400510e40d4f32da2e72168a5eaa3ee28bf6250923603284adfe55af"
#define TEST_O "434e7a05967edaf81ed577ad1956f5d517cf2370517e88c5c3da215c57bedc3f"
#define TEST_P "bcb185fa69812507e12a8852e6a90a2ae830dc8fae81773a3c25dea2a8411ff0"

//A and B can be a point and X and Y can be a point
#define TEST_A "887387e452b8eacc4acfde10d9aaf7f6d9a0f975aabb10d006e4da568744d06c"
#define TEST_B "61de6d95231cd89026e286df3b6ae4a894a3378e393e93a0f45b666329a0ae34"
#define TEST_X "deadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeef12345678"
#define TEST_Y "ffe4951c17e5edff2bb8f4a066b4bee9a962e60e709677d1cb376b38ea0d3bd8"

#define TEST_Z "ec208baa0fc1c19f708a9ca96fdeff3ac3f230bb4a7ba4aede4942ad003c0f60"
#define TEST_R "ac8d1c87e51d0d441be8b3dd5b05c8795b48875dffe00b7ffcfac23010d3a395"
#define TEST_S "68342ceff8935ededd102dd876ffd6ba72d6a427a3edb13d26eb0781cb423c4"

int tests_run = 0;

static char* test_S256Field_add() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    mpz_t expected_result;
    mpz_init_set_str(expected_result, "de92540bf693acef0a66b39045627dd98810a4fe4f4436c1a9e52a28fc419621", 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* test_y = S256Field_init(test_Y);
    S256Field* result = S256Field_add(test_x, test_y);
    mu_assert("Error: S256Field_add doesn't work", mpz_cmp(result->num, expected_result) == 0);
    S256Field_free(result);
    S256Field_free(test_x);
    S256Field_free(test_y);
    mpz_clear(expected_result);
    return 0;
}

static char* test_S256Field_sub() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    mpz_t expected_result;
    mpz_init_set_str(expected_result, "dec929d3c6c7d0f0b2f4ca4f77f90006354ad8e16e17471e137653b5282716cf", 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* test_y = S256Field_init(test_Y);
    S256Field* result = S256Field_sub(test_x, test_y);
    mu_assert("Error: S256Field_sub doesn't work", mpz_cmp(result->num, expected_result) == 0);
    S256Field_free(result);
    S256Field_free(test_x);
    S256Field_free(test_y);
    mpz_clear(expected_result);
    return 0;
}

static char* test_S256Field_mul() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    mpz_t expected_result;
    mpz_init_set_str(expected_result, "40ff672031c3ee56121ce660b15eb9111b5e97ca27c7beafb50ca874c9f38146", 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* test_y = S256Field_init(test_Y);
    S256Field* result = S256Field_mul(test_x, test_y);
    mu_assert("Error: S256Field_mul doesn't work", mpz_cmp(result->num, expected_result) == 0);
    S256Field_free(result);
    S256Field_free(test_x);
    S256Field_free(test_y);
    mpz_clear(expected_result);
    return 0;
}

static char* test_S256Field_s_mul() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    mpz_t expected_result;
    mpz_init_set_str(expected_result, "d404f0aec405362faa5d716659f90317be0459052290161d04bbec2356c4c30f", 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* test_y = S256Field_init(test_Y);
    S256Field* result = S256Field_s_mul(test_x, test_y);
    mu_assert("Error: S256Field_s_mul doesn't work", mpz_cmp(result->num, expected_result) == 0);
    S256Field_free(result);
    S256Field_free(test_x);
    S256Field_free(test_y);
    mpz_clear(expected_result);
    return 0;
}

static char* test_S256Field_mul_scalar() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    mpz_t expected_result;
    mpz_init_set_str(expected_result, "40ff672031c3ee56121ce660b15eb9111b5e97ca27c7beafb50ca874c9f38146", 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* result = S256Field_mul_scalar(test_x, test_Y);
    mu_assert("Error: S256Field_mul_scalar doesn't work", mpz_cmp(result->num, expected_result) == 0);
    S256Field_free(result);
    S256Field_free(test_x);
    mpz_clear(test_Y);
    mpz_clear(expected_result);
    return 0;
}

static char* test_S256Field_mod_inv() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t expected_result;
    mpz_init_set_str(expected_result, "e14a08e686a1504acb1c1f23fa797e0b81476c6a157c6bfb19b55ac3e66e1d7c", 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* result = S256Field_mod_inv(test_x);
    mu_assert("Error: S256Field_mod_inv doesn't work", mpz_cmp(result->num, expected_result) == 0);
    S256Field_free(result);
    S256Field_free(test_x);
    mpz_clear(expected_result);
    return 0;
}

static char* test_S256Field_s_inv() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t expected_result;
    mpz_init_set_str(expected_result, "5d80dba49a677f43014d4250ba745be4eb6171982bd6205581fe48428ff08657", 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* result = S256Field_s_inv(test_x);
    mu_assert("Error: S256Field_s_inv doesn't work", mpz_cmp(result->num, expected_result) == 0);
    S256Field_free(result);
    S256Field_free(test_x);
    mpz_clear(expected_result);
    return 0;
}

static char* test_S256Field_pow() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    mpz_t expected_result;
    mpz_init_set_str(expected_result, "4b4f242ad52794aa1a88e2dc56ec8032a91e0f857684e7d7d2c0edc2a38e7c87", 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* result = S256Field_pow(test_x, test_Y);
    mu_assert("Error: S256Field_pow doesn't work", mpz_cmp(result->num, expected_result) == 0);
    S256Field_free(result);
    S256Field_free(test_x);
    mpz_clear(test_Y);
    mpz_clear(expected_result);
    return 0;
}

static char* test_S256Field_div() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    mpz_t expected_result;
    mpz_init_set_str(expected_result, "00dcad2ac055d0c9a239d0478b1227de5773683cbf67c6629cd7e6fb18ef5e21", 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* test_y = S256Field_init(test_Y);
    S256Field* result = S256Field_div(test_x, test_y);
    mu_assert("Error: S256Field_div doesn't work", mpz_cmp(result->num, expected_result) == 0);
    S256Field_free(result);
    S256Field_free(test_x);
    S256Field_free(test_y);
    mpz_clear(expected_result);
    return 0;
}

static char* test_S256Field_sqrt() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t expected_result;
    mpz_init_set_str(expected_result, "3fca07c83ee3aef41802d1f79aed277c58d39d83d48e45d48320d10e3c1efd24", 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* result = S256Field_sqrt(test_x);
    mu_assert("Error: S256Field_sqrt doesn't work", mpz_cmp(result->num, expected_result) == 0);
    S256Field_free(result);
    S256Field_free(test_x);
    mpz_clear(expected_result);
    return 0;
}

static char* test_S256Point_add() {
    //Adding two points with different x and y
    mpz_t test_A;
    mpz_init_set_str(test_A, TEST_A, 16);
    mpz_t test_B;
    mpz_init_set_str(test_B, TEST_B, 16);
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    mpz_t expected_x;
    mpz_init_set_str(expected_x, "456620086e0791dd93d1b8338cda34fde782c4c0290041099464de364ef3356a", 16);
    mpz_t expected_y;
    mpz_init_set_str(expected_y, "669b50ea184f441fc623362965e34074856e0cc909b74e291c61f7ddc1f683e2", 16);
    S256Field* test_a = S256Field_init(test_A);
    S256Field* test_b = S256Field_init(test_B);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* test_y = S256Field_init(test_Y);
    S256Point* a = S256Point_init(test_a, test_b);
    S256Point* b = S256Point_init(test_x, test_y);
    S256Point* result = S256Point_add(a, b);
    mu_assert("Error: S256Point_add doesn't work", mpz_cmp(result->x->num, expected_x) == 0);
    mu_assert("Error: S256Point_add doesn't work", mpz_cmp(result->y->num, expected_y) == 0);
    S256Point_free(result);
    S256Point_free(a);
    S256Point_free(b);
    mpz_clear(expected_x);
    mpz_clear(expected_y);
    
    //Adding two points with the same x and different y
    mpz_t test_N1;
    mpz_init_set_str(test_N1, TEST_N, 16);
    mpz_t test_N2;
    mpz_init_set_str(test_N2, TEST_N, 16);
    mpz_t test_O;
    mpz_init_set_str(test_O, TEST_O, 16);
    mpz_t test_P;
    mpz_init_set_str(test_P, TEST_P, 16);
    S256Field* test_n1 = S256Field_init(test_N1);
    S256Field* test_n2 = S256Field_init(test_N2);
    S256Field* test_o = S256Field_init(test_O);
    S256Field* test_p = S256Field_init(test_P);
    S256Point* c = S256Point_init(test_n1, test_o);
    S256Point* d = S256Point_init(test_n2, test_p);
    S256Point* result2 = S256Point_add(c, d);
    mu_assert("Error: S256Point_add doesn't work", result2 == NULL);
    S256Point_free(c);
    S256Point_free(d);

    //Adding two points with the same x and y
    mpz_t test_X1;
    mpz_init_set_str(test_X1, TEST_X, 16);
    mpz_t test_Y1;
    mpz_init_set_str(test_Y1, TEST_Y, 16);
    mpz_t test_X2;
    mpz_init_set_str(test_X2, TEST_X, 16);
    mpz_t test_Y2;
    mpz_init_set_str(test_Y2, TEST_Y, 16);
    mpz_t expected_x2;
    mpz_init_set_str(expected_x2, "4c528d70d1702dab37a7e442aa2331ac04884582de6f0fb23285d0ff8b1d9b8f", 16);
    mpz_t expected_y2;
    mpz_init_set_str(expected_y2, "e6f30b8af8fc994ed48f11ab8a560bb8d1fd7c4474e738c61a2fcc27398cbe25", 16);
    S256Field* test_x1 = S256Field_init(test_X1);
    S256Field* test_y1 = S256Field_init(test_Y1);
    S256Field* test_x2 = S256Field_init(test_X2);
    S256Field* test_y2 = S256Field_init(test_Y2);
    S256Point* e = S256Point_init(test_x1, test_y1);
    S256Point* f = S256Point_init(test_x2, test_y2);
    S256Point* result3 = S256Point_add(e, f);
    mu_assert("Error: S256Point_add doesn't work", mpz_cmp(result3->x->num, expected_x2) == 0);
    mu_assert("Error: S256Point_add doesn't work", mpz_cmp(result3->y->num, expected_y2) == 0);
    S256Point_free(result3);
    S256Point_free(e);
    S256Point_free(f);
    mpz_clear(expected_x2);
    mpz_clear(expected_y2);
    return 0;
}

static char* test_S256Point_mul() {
    mpz_t test_A;
    mpz_init_set_str(test_A, TEST_A, 16);
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    mpz_t expected_x;
    mpz_init_set_str(expected_x, "d148d491f77e6546f6a2f371727a5dc6e8034fe116eecb405fc4b84feeaed996", 16);
    mpz_t expected_y;
    mpz_init_set_str(expected_y, "35c349a4272354858ef3be984c31e16d09282c872ba9189a93237f54c56dc5c8", 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* test_y = S256Field_init(test_Y);
    S256Point* b = S256Point_init(test_x, test_y);
    S256Point* result = S256Point_mul(b, test_A);
    mu_assert("Error: S256Point_mul doesn't work", mpz_cmp(result->x->num, expected_x) == 0);
    mu_assert("Error: S256Point_mul doesn't work", mpz_cmp(result->y->num, expected_y) == 0);
    mpz_clear(test_A);
    mpz_clear(expected_x);
    mpz_clear(expected_y);
    S256Point_free(b);
    S256Point_free(result);
    return 0;
}

static char* test_S256Point_verify() {
    mpz_t test_A;
    mpz_init_set_str(test_A, TEST_A, 16);
    mpz_t test_B;
    mpz_init_set_str(test_B, TEST_B, 16);
    mpz_t test_Z;
    mpz_init_set_str(test_Z, TEST_Z, 16);
    mpz_t test_R;
    mpz_init_set_str(test_R, TEST_R, 16);
    mpz_t test_S;
    mpz_init_set_str(test_S, TEST_S, 16);
    S256Field* test_z = S256Field_init(test_Z);
    S256Field* test_r = S256Field_init(test_R);
    S256Field* test_s = S256Field_init(test_S);
    S256Field* test_a = S256Field_init(test_A);
    S256Field* test_b = S256Field_init(test_B);
    Signature* test_sig = Signature_init(test_r, test_s);
    S256Point* test_p = S256Point_init(test_a, test_b);
    int result = S256Point_verify(test_p, test_z, test_sig);
    mu_assert("Error: S256Point_verify doesn't work", result == 1);
    S256Field_free(test_z);
    Signature_free(test_sig);
    S256Point_free(test_p);
    return 0;
}

static char* test_Deterministic_k() {
    mpz_t test_Z;
    mpz_init_set_str(test_Z, TEST_Z, 16);
    S256Field* test_z = S256Field_init(test_Z);
    PrivateKey* test_key = PrivateKey_init("test secret");
    S256Field* test_k = Deterministic_k(test_key, test_z);
    mpz_t expected_k;
    mpz_init_set_str(expected_k, "76710a244f87c31c7e6024b6f7786908e49a82a658d3186ec3631a95350287e1", 16);
    mu_assert("Error: Deterministic_k doesn't work", mpz_cmp(test_k->num, expected_k) == 0);
    PrivateKey_free(test_key);
    S256Field_free(test_k);
    S256Field_free(test_z);
    mpz_clear(expected_k);
    return 0;
}

static char* test_PrivateKey_sign() {
    mpz_t test_Z;
    mpz_init_set_str(test_Z, TEST_Z, 16);
    S256Field* test_z = S256Field_init(test_Z);
    PrivateKey* test_key = PrivateKey_init("test secret");
    Signature* test_sig = PrivateKey_sign(test_key, test_z);
    mpz_t expected_r;
    mpz_init_set_str(expected_r, "efed461e09407dabdd08cedcbc31f1f421572a7a32077d16b448fb0bf1237028", 16);
    mpz_t expected_s;
    mpz_init_set_str(expected_s, "3fbf0cd46391e0a60c592f078354e9011b5218d5e741b04dfa6f688293ff3e95", 16);
    mu_assert("Error: PrivateKey_sign doesn't work", mpz_cmp(test_sig->r->num, expected_r) == 0);
    mu_assert("Error: PrivateKey_sign doesn't work", mpz_cmp(test_sig->s->num, expected_s) == 0);
    Signature_free(test_sig);
    PrivateKey_free(test_key);
    S256Field_free(test_z);
    mpz_clear(expected_r);
    mpz_clear(expected_s);
    return 0;
}

static char* test_S256Point_sec_uncompressed() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* test_y = S256Field_init(test_Y);
    S256Point* test_p = S256Point_init(test_x, test_y);
    unsigned char result[65];
    S256Point_sec_uncompressed(test_p, result);
    // for (int i = 0; i < 65; i++) {
    //     printf("%02x", result[i]);
    // }
    // printf("\n");
    unsigned char expected_result[] = {0x04, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0x12, 0x34, 0x56, 0x78, 0xff, 0xe4, 0x95, 0x1c, 0x17, 0xe5, 0xed, 0xff, 0x2b, 0xb8, 0xf4, 0xa0, 0x66, 0xb4, 0xbe, 0xe9, 0xa9, 0x62, 0xe6, 0x0e, 0x70, 0x96, 0x77, 0xd1, 0xcb, 0x37, 0x6b, 0x38, 0xea, 0x0d, 0x3b, 0xd8};
    mu_assert("Error: S256Point_sec_uncompressed doesn't work", memcmp(result, expected_result, sizeof(expected_result)) == 0);
    S256Point_free(test_p);
    return 0;
}

static char* test_S256Point_sec_compressed() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* test_y = S256Field_init(test_Y);
    S256Point* test_p_even = S256Point_init(test_x, test_y);
    unsigned char result_even[33];
    S256Point_sec_compressed(test_p_even, result_even);
    mpz_t test_N;
    mpz_init_set_str(test_N, TEST_N, 16);
    mpz_t test_O;
    mpz_init_set_str(test_O, TEST_O, 16);
    S256Field* test_n = S256Field_init(test_N);
    S256Field* test_o = S256Field_init(test_O);
    S256Point* test_p_odd = S256Point_init(test_n, test_o);
    unsigned char result_odd[33];
    S256Point_sec_compressed(test_p_odd, result_odd);
    unsigned char expected_result_even[] = {0x02, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0x12, 0x34, 0x56, 0x78};
    unsigned char expected_result_odd[] = {0x03, 0x6d, 0x18, 0x3d, 0xe4, 0x40, 0x05, 0x10, 0xe4, 0x0d, 0x4f, 0x32, 0xda, 0x2e, 0x72, 0x16, 0x8a, 0x5e, 0xaa, 0x3e, 0xe2, 0x8b, 0xf6, 0x25, 0x09, 0x23, 0x60, 0x32, 0x84, 0xad, 0xfe, 0x55, 0xaf};
    mu_assert("Error: S256Point_sec_compressed doesn't work", memcmp(result_even, expected_result_even, sizeof(expected_result_even)) == 0);
    mu_assert("Error: S256Point_sec_compressed doesn't work", memcmp(result_odd, expected_result_odd, sizeof(expected_result_odd)) == 0);
    S256Point_free(test_p_even);
    S256Point_free(test_p_odd);
    return 0;
}

static char* test_S256Point_parse_sec() {
    unsigned char test_compressed_sec_even[] = {0x02, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0x12, 0x34, 0x56, 0x78};
    S256Point* test_compressed_even_result = S256Point_parse_sec(test_compressed_sec_even);
    unsigned char test_compressed_sec_odd[] = {0x03, 0x6d, 0x18, 0x3d, 0xe4, 0x40, 0x05, 0x10, 0xe4, 0x0d, 0x4f, 0x32, 0xda, 0x2e, 0x72, 0x16, 0x8a, 0x5e, 0xaa, 0x3e, 0xe2, 0x8b, 0xf6, 0x25, 0x09, 0x23, 0x60, 0x32, 0x84, 0xad, 0xfe, 0x55, 0xaf};
    S256Point* test_compressed_odd_result = S256Point_parse_sec(test_compressed_sec_odd);
    unsigned char test_uncompressed_sec[] = {0x04, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0x12, 0x34, 0x56, 0x78, 0xff, 0xe4, 0x95, 0x1c, 0x17, 0xe5, 0xed, 0xff, 0x2b, 0xb8, 0xf4, 0xa0, 0x66, 0xb4, 0xbe, 0xe9, 0xa9, 0x62, 0xe6, 0x0e, 0x70, 0x96, 0x77, 0xd1, 0xcb, 0x37, 0x6b, 0x38, 0xea, 0x0d, 0x3b, 0xd8};
    S256Point* test_uncompressed_result = S256Point_parse_sec(test_uncompressed_sec);
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* test_y = S256Field_init(test_Y);
    S256Point* expected_result = S256Point_init(test_x, test_y);
    mpz_t test_N;
    mpz_init_set_str(test_N, TEST_N, 16);
    mpz_t test_O;
    mpz_init_set_str(test_O, TEST_O, 16);
    S256Field* test_n = S256Field_init(test_N);
    S256Field* test_o = S256Field_init(test_O);
    S256Point* expected_odd_result = S256Point_init(test_n, test_o);
    mu_assert("Error: S256Point_parse_sec doesn't work", S256Point_eq(test_uncompressed_result, expected_result) && S256Point_eq(test_compressed_even_result, expected_result) && S256Point_eq(test_compressed_odd_result, expected_odd_result));
    S256Point_free(test_compressed_even_result);
    S256Point_free(test_compressed_odd_result);
    S256Point_free(test_uncompressed_result);
    S256Point_free(expected_result);
    S256Point_free(expected_odd_result);
    return 0;
}

static char* all_tests() {
    //S256Field tests
    mu_run_test(test_S256Field_add);
    mu_run_test(test_S256Field_sub);
    mu_run_test(test_S256Field_mul);
    mu_run_test(test_S256Field_s_mul);
    mu_run_test(test_S256Field_mul_scalar);
    mu_run_test(test_S256Field_mod_inv);
    mu_run_test(test_S256Field_s_inv);
    mu_run_test(test_S256Field_pow);
    mu_run_test(test_S256Field_div);
    mu_run_test(test_S256Field_sqrt);
    
    //S256Point tests
    mu_run_test(test_S256Point_add);
    // mu_run_test(test_S256Point_mul);
    // mu_run_test(test_S256Point_verify);

    // //Private Key tests
    // mu_run_test(test_Deterministic_k);
    // mu_run_test(test_PrivateKey_sign);
    mu_run_test(test_S256Point_sec_uncompressed);
    mu_run_test(test_S256Point_sec_compressed);
    mu_run_test(test_S256Point_parse_sec);
    return 0;
}

int main(int argc, char **argv) {
    Initialize_prime();

    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    } else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    Free_prime();

    return result != 0;
}