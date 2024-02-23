#include <stdio.h>

#include "minunit.h"

#include "../bitcoin/ecc/s256field.h"

#define TEST_X "deadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeef12345678"
#define TEST_Y "deadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeef87654321"

int tests_run = 0;

static char* test_S256Field_add() {
    mpz_t test_X;
    mpz_init_set_str(test_X, TEST_X, 16);
    mpz_t test_Y;
    mpz_init_set_str(test_Y, TEST_Y, 16);
    mpz_t expected_result;
    mpz_init_set_str(expected_result, "bd5b7ddfbd5b7ddfbd5b7ddfbd5b7ddfbd5b7ddfbd5b7ddfbd5b7ddf99999d6a", 16);
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
    mpz_init_set_str(expected_result, "fffffffffffffffffffffffffffffffffffffffffffffffffffffffe8acf0f86", 16);
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
    mpz_init_set_str(expected_result, "d3d21fc99ecd55cf69c88bd534c3c1daffbef7e0caba2de69f5632eee3e23ee5", 16);
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
    mpz_init_set_str(expected_result, "278e3e90f9b3c6a59b844089134b3ff7e336368d6f678377a21d7e325c2a87f6", 16);
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
    mpz_init_set_str(expected_result, "d3d21fc99ecd55cf69c88bd534c3c1daffbef7e0caba2de69f5632eee3e23ee5", 16);
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
    mpz_init_set_str(expected_result, "9057793362cac928fcd39fc36b14e325635c5946ed07360c12a5840fb7be2955", 16);
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
    mpz_init_set_str(expected_result, "e2c92b6285e51e3e9e338dab13944c5bdb12eed853a5b39bb273f0a9b65dbd00", 16);
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

static char* all_tests() {
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