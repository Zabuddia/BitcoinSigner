#include <stdio.h>

#include "minunit.h"

#include "../bitcoin/ecc/s256field.h"
#include "../bitcoin/ecc/s256point.h"
#include "../bitcoin/ecc/signature.h"
#include "../bitcoin/ecc/privatekey.h"
#include "../bitcoin/tx/tx.h"
#include "../bitcoin/tx/txin.h"
#include "../bitcoin/tx/txout.h"

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

//For testing signature DER format
#define TEST_DER_R "37206a0610995c58074999cb9767b87af4c4978db68c06e8e6e81d282047a7c6"
#define TEST_DER_S "8ca63759c1157ebeaec0d03cecca119fc9a75bf8e6d0fa65c841c8e2738cdaec"

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

static char* test_Signature_der() {
    mpz_t test_R1;
    mpz_init_set_str(test_R1, TEST_DER_R, 16);
    mpz_t test_S1;
    mpz_init_set_str(test_S1, TEST_DER_S, 16);
    mpz_t test_R2;
    mpz_init_set_str(test_R2, TEST_R, 16);
    mpz_t test_S2;
    mpz_init_set_str(test_S2, TEST_S, 16);
    S256Field* test_r1 = S256Field_init(test_R1);
    S256Field* test_s1 = S256Field_init(test_S1);
    Signature* test_sig1 = Signature_init(test_r1, test_s1);
    S256Field* test_r2 = S256Field_init(test_R2);
    S256Field* test_s2 = S256Field_init(test_S2);
    Signature* test_sig2 = Signature_init(test_r2, test_s2);
    unsigned char result1[72] = {0};
    unsigned char result2[72] = {0};
    Signature_der(test_sig1, result1);
    Signature_der(test_sig2, result2);
    unsigned char expected_result1[72] = {0x30, 0x45, 0x02, 0x20, 0x37, 0x20, 0x6a, 0x06, 0x10, 0x99, 0x5c, 0x58, 0x07, 0x49, 0x99, 0xcb, 0x97, 0x67, 0xb8, 0x7a, 0xf4, 0xc4, 0x97, 0x8d, 0xb6, 0x8c, 0x06, 0xe8, 0xe6, 0xe8, 0x1d, 0x28, 0x20, 0x47, 0xa7, 0xc6, 0x02, 0x21, 0x00, 0x8c, 0xa6, 0x37, 0x59, 0xc1, 0x15, 0x7e, 0xbe, 0xae, 0xc0, 0xd0, 0x3c, 0xec, 0xca, 0x11, 0x9f, 0xc9, 0xa7, 0x5b, 0xf8, 0xe6, 0xd0, 0xfa, 0x65, 0xc8, 0x41, 0xc8, 0xe2, 0x73, 0x8c, 0xda, 0xec};
    unsigned char expected_result2[72] = {0x30, 0x45, 0x02, 0x21, 0x00, 0xac, 0x8d, 0x1c, 0x87, 0xe5, 0x1d, 0x0d, 0x44, 0x1b, 0xe8, 0xb3, 0xdd, 0x5b, 0x05, 0xc8, 0x79, 0x5b, 0x48, 0x87, 0x5d, 0xff, 0xe0, 0x0b, 0x7f, 0xfc, 0xfa, 0xc2, 0x30, 0x10, 0xd3, 0xa3, 0x95, 0x02, 0x20, 0x06, 0x83, 0x42, 0xce, 0xff, 0x89, 0x35, 0xed, 0xed, 0xd1, 0x02, 0xdd, 0x87, 0x6f, 0xfd, 0x6b, 0xa7, 0x2d, 0x6a, 0x42, 0x7a, 0x3e, 0xdb, 0x13, 0xd2, 0x6e, 0xb0, 0x78, 0x1c, 0xb4, 0x23, 0xc4};
    mu_assert("Error: Signature_der doesn't work (result 1)", memcmp(result1, expected_result1, sizeof(expected_result1)) == 0);
    mu_assert("Error: Signature_der doesn't work (result 2)", memcmp(result2, expected_result2, sizeof(expected_result2)) == 0);
    Signature_free(test_sig1);
    Signature_free(test_sig2);
    return 0;
}

static char* test_encode_base58() {
    const char test1[] = {0x7c, 0x07, 0x6f, 0xf3, 0x16, 0x69, 0x2a, 0x3d, 0x7e, 0xb3, 0xc3, 0xbb, 0x0f, 0x8b, 0x14, 0x88, 0xcf, 0x72, 0xe1, 0xaf, 0xcd, 0x92, 0x9e, 0x29, 0x30, 0x70, 0x32, 0x99, 0x7a, 0x83, 0x8a, 0x3d};
    unsigned char result1[1024];
    size_t size = 1024;
    encode_base58(result1, &size, test1, sizeof(test1));
    char* expected_result1 = "9MA8fRQrT4u8Zj8ZRd6MAiiyaxb2Y1CMpvVkHQu5hVM6";
    mu_assert("Error: encode_base58 doesn't work (result 1)", strcmp((char*)result1, expected_result1) == 0);
    const char test2[] = {0xef, 0xf6, 0x9e, 0xf2, 0xb1, 0xbd, 0x93, 0xa6, 0x6e, 0xd5, 0x21, 0x9a, 0xdd, 0x4f, 0xb5, 0x1e, 0x11, 0xa8, 0x40, 0xf4, 0x04, 0x87, 0x63, 0x25, 0xa1, 0xe8, 0xff, 0xe0, 0x52, 0x9a, 0x2c};
    unsigned char result2[1024];
    encode_base58(result2, &size, test2, sizeof(test2));
    char* expected_result2 = "4fE3H2E6XMp4SsxtwinF7w9a34ooUrwWe4WsW1458Pd";
    mu_assert("Error: encode_base58 doesn't work (result 2)", strcmp((char*)result2, expected_result2) == 0);
    const char test3[] = {0xc7, 0x20, 0x7f, 0xee, 0x19, 0x7d, 0x27, 0xc6, 0x18, 0xae, 0xa6, 0x21, 0x40, 0x6f, 0x6b, 0xf5, 0xef, 0x6f, 0xca, 0x38, 0x68, 0x1d, 0x82, 0xb2, 0xf0, 0x6f, 0xdd, 0xbd, 0xce, 0x6f, 0xea, 0xb6};
    unsigned char result3[1024];
    encode_base58(result3, &size, test3, sizeof(test3));
    char* expected_result3 = "EQJsjkd6JaGwxrjEhfeqPenqHwrBmPQZjJGNSCHBkcF7";
    mu_assert("Error: encode_base58 doesn't work (result 3)", strcmp((char*)result3, expected_result3) == 0);
    return 0;
}

static char* test_S256Point_address() {
    // PrivateKey* test_key = PrivateKey_init("test secret");
    // char result[1024];
    // S256Point_address(test_key->point, result, FALSE, TRUE);
    // char* expected_result = "oDXeZKUNbGkmjpw9adB79LHZ3w2kGdDtkkmLwjEA36cPD";
    // mu_assert("Error: S256Point_address doesn't work", strcmp(result, expected_result) == 0);
    // PrivateKey_free(test_key);

    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    S256Field* test_x = S256Field_init(test_X);
    S256Field* test_y = S256Field_init(test_Y);
    S256Point* test_p = S256Point_init(test_x, test_y);
    
    unsigned char result1[1024];
    S256Point_address(test_p, result1, FALSE, FALSE);
    char* expected_result1 = "1JjuTbJNnZ7oxmvsMKkwhQcwKZBNB57Dc2";

    unsigned char result2[1024];
    S256Point_address(test_p, result2, TRUE, FALSE);
    char* expected_result2 = "1EmNjxzL2pEeAdkFzJNmU2wzQQHrNFtA9J";

    unsigned char result3[1024];
    S256Point_address(test_p, result3, FALSE, TRUE);
    char* expected_result3 = "myFrkePMbaZ4jtQV4tjKXKqGBYn56ioF2S";

    unsigned char result4[1024];
    S256Point_address(test_p, result4, TRUE, TRUE);
    char* expected_result4 = "muHL325JqqftwkDshsM9HxAKGPtZNLmusA";

    mu_assert("Error: S256Point_address doesn't work", strcmp((char*)result1, expected_result1) == 0);
    mu_assert("Error: S256Point_address doesn't work", strcmp((char*)result2, expected_result2) == 0);
    mu_assert("Error: S256Point_address doesn't work", strcmp((char*)result3, expected_result3) == 0);
    mu_assert("Error: S256Point_address doesn't work", strcmp((char*)result4, expected_result4) == 0);

    S256Point_free(test_p);
    return 0;
}

static char* test_PrivateKey_wif() {
    PrivateKey* test_key = PrivateKey_init("test secret");

    unsigned char result1[1024];
    PrivateKey_wif(test_key, result1, FALSE, FALSE);
    char* expected_result1 = "5K4T5kjMFenTMzWNNwAjBcBrBVwhLqWuWm2MPBfAnb9LnexWEmJ";

    unsigned char result2[1024];
    PrivateKey_wif(test_key, result2, TRUE, FALSE);
    char* expected_result2 = "L2iE7z1vPfmdMepdMp5wBpJt5LkR4Z8znMJxxv3UCDhpL6DGw9pC";

    unsigned char result3[1024];
    PrivateKey_wif(test_key, result3, FALSE, TRUE);
    char* expected_result3 = "92q5fVYtqsrbL41f1H4e4CjoqAJQW146rhtJTp1g8KtPZgBaKKL";

    unsigned char result4[1024];
    PrivateKey_wif(test_key, result4, TRUE, TRUE);
    char* expected_result4 = "cT5Dau1mpjTtX6HtkDu4Z8owha3pj1EgrPTS5LVyhLMpaqPXeAbg";

    mu_assert("Error: PrivateKey_wif doesn't work", strcmp((char*)result1, expected_result1) == 0);
    mu_assert("Error: PrivateKey_wif doesn't work", strcmp((char*)result2, expected_result2) == 0);
    mu_assert("Error: PrivateKey_wif doesn't work", strcmp((char*)result3, expected_result3) == 0);
    mu_assert("Error: PrivateKey_wif doesn't work", strcmp((char*)result4, expected_result4) == 0);

    PrivateKey_free(test_key);
    return 0;
}

static char* generate_testnet_address() {
    PrivateKey* test_key = PrivateKey_init("test secret");
    unsigned char testnet_address[1024];
    S256Point_address(test_key->point, testnet_address, FALSE, TRUE);
    printf("Testnet address: %s\n", testnet_address);
    PrivateKey_free(test_key);
    return 0;
}

static char* test_little_endian_to_int() {
    unsigned char test1[] = {0x01, 0x00, 0x00, 0x00};
    mu_assert("Error: little_endian_to_int doesn't work", little_endian_to_int(test1, 4) == 1);
    unsigned char test2[] = {0x01, 0x01, 0x01, 0x01};
    mu_assert("Error: little_endian_to_int doesn't work", little_endian_to_int(test2, 4) == 16843009);
    unsigned char test3[] = {0xff, 0xff, 0xff};
    mu_assert("Error: little_endian_to_int doesn't work", little_endian_to_int(test3, 3) == 16777215);
    return 0;
}

static char* test_int_to_little_endian() {
    unsigned char result1[4];
    int_to_little_endian(1, result1, 4);
    unsigned char expected_result1[] = {0x01, 0x00, 0x00, 0x00};
    mu_assert("Error: int_to_little_endian doesn't work", memcmp(result1, expected_result1, sizeof(expected_result1)) == 0);
    unsigned char result2[4];
    int_to_little_endian(16843009, result2, 4);
    unsigned char expected_result2[] = {0x01, 0x01, 0x01, 0x01};
    mu_assert("Error: int_to_little_endian doesn't work", memcmp(result2, expected_result2, sizeof(expected_result2)) == 0);
    unsigned char result3[3];
    int_to_little_endian(16777215, result3, 3);
    unsigned char expected_result3[] = {0xff, 0xff, 0xff};
    mu_assert("Error: int_to_little_endian doesn't work", memcmp(result3, expected_result3, sizeof(expected_result3)) == 0);
    return 0;
}

static char* test_little_endian_to_long() {
    unsigned char test1[] = {0x01, 0x00, 0x00, 0x00};
    mu_assert("Error: little_endian_to_long doesn't work", little_endian_to_long(test1, 4) == 1);
    unsigned char test2[] = {0x01, 0x01, 0x01, 0x01};
    mu_assert("Error: little_endian_to_long doesn't work", little_endian_to_long(test2, 4) == 16843009);
    unsigned char test3[] = {0xff, 0xff, 0xff};
    mu_assert("Error: little_endian_to_long doesn't work", little_endian_to_long(test3, 3) == 16777215);
    unsigned char test4[] = {0x6d, 0xc7, 0xed, 0x3e, 0x60, 0x10, 0x00, 0x00};
    mu_assert("Error: little_endian_to_long doesn't work", little_endian_to_long(test4, 8) == 18005558675309);
    return 0;
}

static char* test_long_to_little_endian() {
    unsigned char result1[4];
    long_to_little_endian(1, result1, 4);
    unsigned char expected_result1[] = {0x01, 0x00, 0x00, 0x00};
    mu_assert("Error: long_to_little_endian doesn't work", memcmp(result1, expected_result1, sizeof(expected_result1)) == 0);
    unsigned char result2[4];
    long_to_little_endian(16843009, result2, 4);
    unsigned char expected_result2[] = {0x01, 0x01, 0x01, 0x01};
    mu_assert("Error: long_to_little_endian doesn't work", memcmp(result2, expected_result2, sizeof(expected_result2)) == 0);
    unsigned char result3[3];
    long_to_little_endian(16777215, result3, 3);
    unsigned char expected_result3[] = {0xff, 0xff, 0xff};
    mu_assert("Error: long_to_little_endian doesn't work", memcmp(result3, expected_result3, sizeof(expected_result3)) == 0);
    unsigned char result4[8];
    long_to_little_endian(18005558675309, result4, 8);
    unsigned char expected_result4[] = {0x6d, 0xc7, 0xed, 0x3e, 0x60, 0x10, 0x00, 0x00};
    mu_assert("Error: long_to_little_endian doesn't work", memcmp(result4, expected_result4, sizeof(expected_result4)) == 0);
    return 0;
}

static char* test_Tx_parse_version() {
    unsigned char raw_tx[] = {0x01, 0x00, 0x00, 0x00, 0x01, 0x81, 0x3f, 0x79, 0x01, 0x1a, 0xcb, 0x80, 0x92, 0x5d, 0xfe, 0x69, 0xb3, 0xde, 0xf3, 0x55, 0xfe, 0x91, 0x4b, 0xd1, 0xd9, 0x6a, 0x3f, 0x5f, 0x71, 0xbf, 0x83, 0x03, 0xc6, 0xa9, 0x89, 0xc7, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x6b, 0x48, 0x30, 0x45, 0x02, 0x21, 0x00, 0xed, 0x81, 0xff, 0x19, 0x2e, 0x75, 0xa3, 0xfd, 0x23, 0x04, 0x00, 0x4d, 0xca, 0xdb, 0x74, 0x6f, 0xa5, 0xe2, 0x4c, 0x50, 0x31, 0xcc, 0xfc, 0xf2, 0x13, 0x20, 0xb0, 0x27, 0x74, 0x57, 0xc9, 0x8f, 0x02, 0x20, 0x7a, 0x98, 0x6d, 0x95, 0x5c, 0x6e, 0x0c, 0xb3, 0x5d, 0x44, 0x6a, 0x89, 0xd3, 0xf5, 0x61, 0x00, 0xf4, 0xd7, 0xf6, 0x78, 0x01, 0xc3, 0x19, 0x67, 0x74, 0x3a, 0x9c, 0x8e, 0x10, 0x61, 0x5b, 0xed, 0x01, 0x21, 0x03, 0x49, 0xfc, 0x4e, 0x63, 0x1e, 0x36, 0x24, 0xa5, 0x45, 0xde, 0x3f, 0x89, 0xf5, 0xd8, 0x68, 0x4c, 0x7b, 0x81, 0x38, 0xbd, 0x94, 0xbd, 0xd5, 0x31, 0xd2, 0xe2, 0x13, 0xbf, 0x01, 0x6b, 0x27, 0x8a, 0xfe, 0xff, 0xff, 0xff, 0x02, 0xa1, 0x35, 0xef, 0x01, 0x00, 0x00, 0x00, 0x00, 0x19, 0x76, 0xa9, 0x14, 0xbc, 0x3b, 0x65, 0x4d, 0xca, 0x7e, 0x56, 0xb0, 0x4d, 0xca, 0x18, 0xf2, 0x56, 0x6c, 0xda, 0xf0, 0x2e, 0x8d, 0x9a, 0xda, 0x88, 0xac, 0x99, 0xc3, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x76, 0xa9, 0x14, 0x1c, 0x4b, 0xc7, 0x62, 0xdd, 0x54, 0x23, 0xe3, 0x32, 0x16, 0x67, 0x02, 0xcb, 0x75, 0xf4, 0x0d, 0xf7, 0x9f, 0xea, 0x12, 0x88, 0xac, 0x19, 0x43, 0x06, 0x00};
    Tx* test_tx = Tx_parse(raw_tx, FALSE);
    mu_assert("Error: Tx_parse doesn't work for version", test_tx->version == 1);
    Tx_free(test_tx);
    return 0;
}

static char* test_Tx_parse_inputs() {
    unsigned char raw_tx[] = {0x01, 0x00, 0x00, 0x00, 0x01, 0x81, 0x3f, 0x79, 0x01, 0x1a, 0xcb, 0x80, 0x92, 0x5d, 0xfe, 0x69, 0xb3, 0xde, 0xf3, 0x55, 0xfe, 0x91, 0x4b, 0xd1, 0xd9, 0x6a, 0x3f, 0x5f, 0x71, 0xbf, 0x83, 0x03, 0xc6, 0xa9, 0x89, 0xc7, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x6b, 0x48, 0x30, 0x45, 0x02, 0x21, 0x00, 0xed, 0x81, 0xff, 0x19, 0x2e, 0x75, 0xa3, 0xfd, 0x23, 0x04, 0x00, 0x4d, 0xca, 0xdb, 0x74, 0x6f, 0xa5, 0xe2, 0x4c, 0x50, 0x31, 0xcc, 0xfc, 0xf2, 0x13, 0x20, 0xb0, 0x27, 0x74, 0x57, 0xc9, 0x8f, 0x02, 0x20, 0x7a, 0x98, 0x6d, 0x95, 0x5c, 0x6e, 0x0c, 0xb3, 0x5d, 0x44, 0x6a, 0x89, 0xd3, 0xf5, 0x61, 0x00, 0xf4, 0xd7, 0xf6, 0x78, 0x01, 0xc3, 0x19, 0x67, 0x74, 0x3a, 0x9c, 0x8e, 0x10, 0x61, 0x5b, 0xed, 0x01, 0x21, 0x03, 0x49, 0xfc, 0x4e, 0x63, 0x1e, 0x36, 0x24, 0xa5, 0x45, 0xde, 0x3f, 0x89, 0xf5, 0xd8, 0x68, 0x4c, 0x7b, 0x81, 0x38, 0xbd, 0x94, 0xbd, 0xd5, 0x31, 0xd2, 0xe2, 0x13, 0xbf, 0x01, 0x6b, 0x27, 0x8a, 0xfe, 0xff, 0xff, 0xff, 0x02, 0xa1, 0x35, 0xef, 0x01, 0x00, 0x00, 0x00, 0x00, 0x19, 0x76, 0xa9, 0x14, 0xbc, 0x3b, 0x65, 0x4d, 0xca, 0x7e, 0x56, 0xb0, 0x4d, 0xca, 0x18, 0xf2, 0x56, 0x6c, 0xda, 0xf0, 0x2e, 0x8d, 0x9a, 0xda, 0x88, 0xac, 0x99, 0xc3, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x76, 0xa9, 0x14, 0x1c, 0x4b, 0xc7, 0x62, 0xdd, 0x54, 0x23, 0xe3, 0x32, 0x16, 0x67, 0x02, 0xcb, 0x75, 0xf4, 0x0d, 0xf7, 0x9f, 0xea, 0x12, 0x88, 0xac, 0x19, 0x43, 0x06, 0x00};
    Tx* test_tx = Tx_parse(raw_tx, FALSE);
    mu_assert("Error: Tx_parse doesn't work for num_inputs", test_tx->num_inputs == 1);
    unsigned char want1[32] = {0xd1, 0xc7, 0x89, 0xa9, 0xc6, 0x03, 0x83, 0xbf, 0x71, 0x5f, 0x3f, 0x6a, 0xd9, 0xd1, 0x4b, 0x91, 0xfe, 0x55, 0xf3, 0xde, 0xb3, 0x69, 0xfe, 0x5d, 0x92, 0x80, 0xcb, 0x1a, 0x01, 0x79, 0x3f, 0x81};
    mu_assert("Error: Tx_parse doesn't work for prev_tx", memcmp(test_tx->tx_ins[0]->prev_tx, want1, 32) == 0);
    mu_assert("Error: Tx_parse doesn't work for prev_index", test_tx->tx_ins[0]->prev_index == 0);
    unsigned char want2[] = {0x6b, 0x48, 0x30, 0x45, 0x02, 0x21, 0x00, 0xed, 0x81, 0xff, 0x19, 0x2e, 0x75, 0xa3, 0xfd, 0x23, 0x04, 0x00, 0x4d, 0xca, 0xdb, 0x74, 0x6f, 0xa5, 0xe2, 0x4c, 0x50, 0x31, 0xcc, 0xfc, 0xf2, 0x13, 0x20, 0xb0, 0x27, 0x74, 0x57, 0xc9, 0x8f, 0x02, 0x20, 0x7a, 0x98, 0x6d, 0x95, 0x5c, 0x6e, 0x0c, 0xb3, 0x5d, 0x44, 0x6a, 0x89, 0xd3, 0xf5, 0x61, 0x00, 0xf4, 0xd7, 0xf6, 0x78, 0x01, 0xc3, 0x19, 0x67, 0x74, 0x3a, 0x9c, 0x8e, 0x10, 0x61, 0x5b, 0xed, 0x01, 0x21, 0x03, 0x49, 0xfc, 0x4e, 0x63, 0x1e, 0x36, 0x24, 0xa5, 0x45, 0xde, 0x3f, 0x89, 0xf5, 0xd8, 0x68, 0x4c, 0x7b, 0x81, 0x38, 0xbd, 0x94, 0xbd, 0xd5, 0x31, 0xd2, 0xe2, 0x13, 0xbf, 0x01, 0x6b, 0x27, 0x8a};
    mu_assert("Error: Tx_parse doesn't work for script_sig", memcmp(test_tx->tx_ins[0]->script_sig, want2, 108) == 0);
    mu_assert("Error: Tx_parse doesn't work for sequence", test_tx->tx_ins[0]->sequence == 0xfffffffe);
    Tx_free(test_tx);
    return 0;
}

static char* test_Tx_parse_outputs() {
    unsigned char raw_tx[] = {0x01, 0x00, 0x00, 0x00, 0x01, 0x81, 0x3f, 0x79, 0x01, 0x1a, 0xcb, 0x80, 0x92, 0x5d, 0xfe, 0x69, 0xb3, 0xde, 0xf3, 0x55, 0xfe, 0x91, 0x4b, 0xd1, 0xd9, 0x6a, 0x3f, 0x5f, 0x71, 0xbf, 0x83, 0x03, 0xc6, 0xa9, 0x89, 0xc7, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x6b, 0x48, 0x30, 0x45, 0x02, 0x21, 0x00, 0xed, 0x81, 0xff, 0x19, 0x2e, 0x75, 0xa3, 0xfd, 0x23, 0x04, 0x00, 0x4d, 0xca, 0xdb, 0x74, 0x6f, 0xa5, 0xe2, 0x4c, 0x50, 0x31, 0xcc, 0xfc, 0xf2, 0x13, 0x20, 0xb0, 0x27, 0x74, 0x57, 0xc9, 0x8f, 0x02, 0x20, 0x7a, 0x98, 0x6d, 0x95, 0x5c, 0x6e, 0x0c, 0xb3, 0x5d, 0x44, 0x6a, 0x89, 0xd3, 0xf5, 0x61, 0x00, 0xf4, 0xd7, 0xf6, 0x78, 0x01, 0xc3, 0x19, 0x67, 0x74, 0x3a, 0x9c, 0x8e, 0x10, 0x61, 0x5b, 0xed, 0x01, 0x21, 0x03, 0x49, 0xfc, 0x4e, 0x63, 0x1e, 0x36, 0x24, 0xa5, 0x45, 0xde, 0x3f, 0x89, 0xf5, 0xd8, 0x68, 0x4c, 0x7b, 0x81, 0x38, 0xbd, 0x94, 0xbd, 0xd5, 0x31, 0xd2, 0xe2, 0x13, 0xbf, 0x01, 0x6b, 0x27, 0x8a, 0xfe, 0xff, 0xff, 0xff, 0x02, 0xa1, 0x35, 0xef, 0x01, 0x00, 0x00, 0x00, 0x00, 0x19, 0x76, 0xa9, 0x14, 0xbc, 0x3b, 0x65, 0x4d, 0xca, 0x7e, 0x56, 0xb0, 0x4d, 0xca, 0x18, 0xf2, 0x56, 0x6c, 0xda, 0xf0, 0x2e, 0x8d, 0x9a, 0xda, 0x88, 0xac, 0x99, 0xc3, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x76, 0xa9, 0x14, 0x1c, 0x4b, 0xc7, 0x62, 0xdd, 0x54, 0x23, 0xe3, 0x32, 0x16, 0x67, 0x02, 0xcb, 0x75, 0xf4, 0x0d, 0xf7, 0x9f, 0xea, 0x12, 0x88, 0xac, 0x19, 0x43, 0x06, 0x00};
    Tx* test_tx = Tx_parse(raw_tx, FALSE);
    mu_assert("Error: Tx_parse doesn't work for num_outputs", test_tx->num_outputs == 2);
    unsigned long long want1 = 32454049;
    mu_assert("Error: Tx_parse doesn't work for amount", test_tx->tx_outs[0]->amount == want1);
    unsigned char want2[26] = {0x19, 0x76, 0xa9, 0x14, 0xbc, 0x3b, 0x65, 0x4d, 0xca, 0x7e, 0x56, 0xb0, 0x4d, 0xca, 0x18, 0xf2, 0x56, 0x6c, 0xda, 0xf0, 0x2e, 0x8d, 0x9a, 0xda, 0x88, 0xac};
    mu_assert("Error: Tx_parse doesn't work for script_pubkey", memcmp(test_tx->tx_outs[0]->script_pubkey, want2, 26) == 0);
    unsigned long long want3 = 10011545;
    mu_assert("Error: Tx_parse doesn't work for amount", test_tx->tx_outs[1]->amount == want3);
    unsigned char want4[26] = {0x19, 0x76, 0xa9, 0x14, 0x1c, 0x4b, 0xc7, 0x62, 0xdd, 0x54, 0x23, 0xe3, 0x32, 0x16, 0x67, 0x02, 0xcb, 0x75, 0xf4, 0x0d, 0xf7, 0x9f, 0xea, 0x12, 0x88, 0xac};
    mu_assert("Error: Tx_parse doesn't work for script_pubkey", memcmp(test_tx->tx_outs[1]->script_pubkey, want4, 26) == 0);
    Tx_free(test_tx);
    return 0;
}

static char* test_Tx_parse_locktime() {
    unsigned char raw_tx[] = {0x01, 0x00, 0x00, 0x00, 0x01, 0x81, 0x3f, 0x79, 0x01, 0x1a, 0xcb, 0x80, 0x92, 0x5d, 0xfe, 0x69, 0xb3, 0xde, 0xf3, 0x55, 0xfe, 0x91, 0x4b, 0xd1, 0xd9, 0x6a, 0x3f, 0x5f, 0x71, 0xbf, 0x83, 0x03, 0xc6, 0xa9, 0x89, 0xc7, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x6b, 0x48, 0x30, 0x45, 0x02, 0x21, 0x00, 0xed, 0x81, 0xff, 0x19, 0x2e, 0x75, 0xa3, 0xfd, 0x23, 0x04, 0x00, 0x4d, 0xca, 0xdb, 0x74, 0x6f, 0xa5, 0xe2, 0x4c, 0x50, 0x31, 0xcc, 0xfc, 0xf2, 0x13, 0x20, 0xb0, 0x27, 0x74, 0x57, 0xc9, 0x8f, 0x02, 0x20, 0x7a, 0x98, 0x6d, 0x95, 0x5c, 0x6e, 0x0c, 0xb3, 0x5d, 0x44, 0x6a, 0x89, 0xd3, 0xf5, 0x61, 0x00, 0xf4, 0xd7, 0xf6, 0x78, 0x01, 0xc3, 0x19, 0x67, 0x74, 0x3a, 0x9c, 0x8e, 0x10, 0x61, 0x5b, 0xed, 0x01, 0x21, 0x03, 0x49, 0xfc, 0x4e, 0x63, 0x1e, 0x36, 0x24, 0xa5, 0x45, 0xde, 0x3f, 0x89, 0xf5, 0xd8, 0x68, 0x4c, 0x7b, 0x81, 0x38, 0xbd, 0x94, 0xbd, 0xd5, 0x31, 0xd2, 0xe2, 0x13, 0xbf, 0x01, 0x6b, 0x27, 0x8a, 0xfe, 0xff, 0xff, 0xff, 0x02, 0xa1, 0x35, 0xef, 0x01, 0x00, 0x00, 0x00, 0x00, 0x19, 0x76, 0xa9, 0x14, 0xbc, 0x3b, 0x65, 0x4d, 0xca, 0x7e, 0x56, 0xb0, 0x4d, 0xca, 0x18, 0xf2, 0x56, 0x6c, 0xda, 0xf0, 0x2e, 0x8d, 0x9a, 0xda, 0x88, 0xac, 0x99, 0xc3, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x76, 0xa9, 0x14, 0x1c, 0x4b, 0xc7, 0x62, 0xdd, 0x54, 0x23, 0xe3, 0x32, 0x16, 0x67, 0x02, 0xcb, 0x75, 0xf4, 0x0d, 0xf7, 0x9f, 0xea, 0x12, 0x88, 0xac, 0x19, 0x43, 0x06, 0x00};
    Tx* test_tx = Tx_parse(raw_tx, FALSE);
    mu_assert("Error: Tx_parse doesn't work for locktime", test_tx->locktime == 410393);
    Tx_free(test_tx);
    return 0;
}

static char* test_print_formatted_bytes() {
    const unsigned char* hex_string = "1976a9141c4bc762dd5423e332166702cb75f40df79fea1288ac";
    print_formatted_bytes(hex_string);
    return 0;
}

static char* test_read_varint() {
    unsigned char data1[] = {0x64};
    unsigned long long num1 = read_varint(data1);
    mu_assert("Error: read_varint doesn't work", num1 == 100);
    unsigned char data2[] = {0xfd, 0xff, 0x00};
    unsigned long long num2 = read_varint(data2);
    mu_assert("Error: read_varint doesn't work", num2 == 255);
    unsigned char data3[] = {0xfd, 0x2b, 0x02};
    unsigned long long num3 = read_varint(data3);
    mu_assert("Error: read_varint doesn't work", num3 == 555);
    unsigned char data4[] = {0xfe, 0x7f, 0x11, 0x01, 0x00};
    unsigned long long num4 = read_varint(data4);
    mu_assert("Error: read_varint doesn't work", num4 == 70015);
    unsigned char data5[] = {0xff, 0x6d, 0xc7, 0xed, 0x3e, 0x60, 0x10, 0x00, 0x00};
    unsigned long long num5 = read_varint(data5);
    mu_assert("Error: read_varint doesn't work", num5 == 18005558675309);
    return 0;
}

static char* test_encode_varint() {
    unsigned long long num1 = 100;
    unsigned char result1[1024];
    encode_varint(result1, num1);
    unsigned char expected_result1[] = {0x64};
    mu_assert("Error: encode_varint doesn't work", memcmp(result1, expected_result1, sizeof(expected_result1)) == 0);
    unsigned long long num2 = 255;
    unsigned char result2[1024];
    encode_varint(result2, num2);
    unsigned char expected_result2[] = {0xfd, 0xff, 0x00};
    mu_assert("Error: encode_varint doesn't work", memcmp(result2, expected_result2, sizeof(expected_result2)) == 0);
    unsigned long long num3 = 555;
    unsigned char result3[1024];
    encode_varint(result3, num3);
    unsigned char expected_result3[] = {0xfd, 0x2b, 0x02};
    mu_assert("Error: encode_varint doesn't work", memcmp(result3, expected_result3, sizeof(expected_result3)) == 0);
    unsigned long long num4 = 70015;
    unsigned char result4[1024];
    encode_varint(result4, num4);
    unsigned char expected_result4[] = {0xfe, 0x7f, 0x11, 0x01, 0x00};
    mu_assert("Error: encode_varint doesn't work", memcmp(result4, expected_result4, sizeof(expected_result4)) == 0);
    unsigned long long num5 = 18005558675309;
    unsigned char result5[1024];
    encode_varint(result5, num5);
    unsigned char expected_result5[] = {0xff, 0x6d, 0xc7, 0xed, 0x3e, 0x60, 0x10, 0x00, 0x00};
    mu_assert("Error: encode_varint doesn't work", memcmp(result5, expected_result5, sizeof(expected_result5)) == 0);
    return 0;
}

static char* test_little_endian_to_big_endian() {
    unsigned char test1[] = {0x01, 0x00, 0x00, 0x00};
    little_endian_to_big_endian(test1, 4);
    unsigned char expected_result1[] = {0x00, 0x00, 0x00, 0x01};
    mu_assert("Error: little_endian_to_big_endian doesn't work", memcmp(test1, expected_result1, sizeof(expected_result1)) == 0);
    unsigned char test2[] = {0x01, 0x01, 0x01, 0x01};
    little_endian_to_big_endian(test2, 4);
    unsigned char expected_result2[] = {0x01, 0x01, 0x01, 0x01};
    mu_assert("Error: little_endian_to_big_endian doesn't work", memcmp(test2, expected_result2, sizeof(expected_result2)) == 0);
    unsigned char test3[] = {0xff, 0xff, 0xff};
    little_endian_to_big_endian(test3, 3);
    unsigned char expected_result3[] = {0xff, 0xff, 0xff};
    mu_assert("Error: little_endian_to_big_endian doesn't work", memcmp(test3, expected_result3, sizeof(expected_result3)) == 0);
    unsigned char test4[] = {0x81, 0x3f, 0x79, 0x01, 0x1a, 0xcb, 0x80, 0x92, 0x5d, 0xfe, 0x69, 0xb3, 0xde, 0xf3, 0x55, 0xfe, 0x91, 0x4b, 0xd1, 0xd9, 0x6a, 0x3f, 0x5f, 0x71, 0xbf, 0x83, 0x03, 0xc6, 0xa9, 0x89, 0xc7, 0xd1};
    little_endian_to_big_endian(test4, 32);
    unsigned char expected_result4[] = {0xd1, 0xc7, 0x89, 0xa9, 0xc6, 0x03, 0x83, 0xbf, 0x71, 0x5f, 0x3f, 0x6a, 0xd9, 0xd1, 0x4b, 0x91, 0xfe, 0x55, 0xf3, 0xde, 0xb3, 0x69, 0xfe, 0x5d, 0x92, 0x80, 0xcb, 0x1a, 0x01, 0x79, 0x3f, 0x81};
    mu_assert("Error: little_endian_to_big_endian doesn't work", memcmp(test4, expected_result4, sizeof(expected_result4)) == 0);
    return 0;
}

static char* all_tests() {
    // //S256Field tests
    // mu_run_test(test_S256Field_add);
    // mu_run_test(test_S256Field_sub);
    // mu_run_test(test_S256Field_mul);
    // mu_run_test(test_S256Field_s_mul);
    // mu_run_test(test_S256Field_mul_scalar);
    // mu_run_test(test_S256Field_mod_inv);
    // mu_run_test(test_S256Field_s_inv);
    // mu_run_test(test_S256Field_pow);
    // mu_run_test(test_S256Field_div);
    // mu_run_test(test_S256Field_sqrt);
    
    // //S256Point tests
    // mu_run_test(test_S256Point_add);
    // mu_run_test(test_S256Point_mul);
    // mu_run_test(test_S256Point_verify);
    // mu_run_test(test_S256Point_sec_uncompressed);
    // mu_run_test(test_S256Point_sec_compressed);
    // mu_run_test(test_S256Point_parse_sec);
    // mu_run_test(test_S256Point_address);

    // //Private Key tests
    // mu_run_test(test_Deterministic_k);
    // mu_run_test(test_PrivateKey_sign);
    // mu_run_test(test_PrivateKey_wif);

    // //Signature tests
    // mu_run_test(test_Signature_der);

    // //Tx tests
    // mu_run_test(test_Tx_parse_version);
    // mu_run_test(test_Tx_parse_inputs);
    // mu_run_test(test_Tx_parse_outputs);
    // mu_run_test(test_Tx_parse_locktime);

    // //Helper tests
    // mu_run_test(test_encode_base58);
    // mu_run_test(test_little_endian_to_int);
    // mu_run_test(test_int_to_little_endian);
    // mu_run_test(test_little_endian_to_long);
    // mu_run_test(test_long_to_little_endian);
    // mu_run_test(test_print_formatted_bytes);
    // mu_run_test(test_read_varint);
    // mu_run_test(test_encode_varint);
    // mu_run_test(test_little_endian_to_big_endian);

    // //Create addresses
    // mu_run_test(generate_testnet_address);

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