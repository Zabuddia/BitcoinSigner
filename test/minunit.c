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

static char* all_tests() {
    mu_run_test(test_S256Field_add);
    mu_run_test(test_S256Field_mul);
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