#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gmp.h>

#include "s256field.h"

mpz_t PRIME;

void Initialize_prime() {
    mpz_init_set_str(PRIME, P, 16);
}

S256Field* S256Field_init(mpz_t num) {
    assert(mpz_cmp_ui(num, 0) >= 0 && mpz_cmp(num, PRIME) < 0); //Check that num is between 0 and prime - 1
    S256Field* element = (S256Field*)malloc(sizeof(S256Field));
    if (element == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    mpz_init_set(element->num, num);
    mpz_init_set(element->prime, PRIME);
    return element;
}

void S256Field_free(S256Field* element) {
    if (element != NULL) {
        mpz_clear(element->num);
        free(element);
    }
}

void S256Field_toString(S256Field* element) {
    gmp_printf("S256Field_%Zd(%Zd)\n", element->prime, element->num);
}

int S256Field_eq(S256Field* e1, S256Field* e2) {
    if (e2 == NULL) {
        return 0;
    }

    if (mpz_cmp(e1->num, e2->num) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int S256Field_ne(S256Field* e1, S256Field* e2) {
    if (mpz_cmp(e1->num, e2->num) != 0) {
        return 1;
    } else {
        return 0;
    }
}

S256Field* S256Field_add(S256Field* e1, S256Field* e2) {
    mpz_t num;
    mpz_t prime;
    mpz_init(num);
    mpz_init_set(prime, e1->prime);
    mpz_add(num, e1->num, e2->num);
    mpz_mod(num, num, prime);
    return S256Field_init(num);
}

S256Field* S256Field_sub(S256Field* e1, S256Field* e2) {
    mpz_t num;
    mpz_t prime;
    mpz_init(num);
    mpz_init_set(prime, e1->prime);
    mpz_sub(num, e1->num, e2->num);
    mpz_add(num, num, prime); //Ensure a positive result
    mpz_mod(num, num, prime);
    return S256Field_init(num);
}

S256Field* S256Field_mul(S256Field* e1, S256Field* e2) {
    mpz_t num;
    mpz_t prime;
    mpz_init(num);
    mpz_init_set(prime, e1->prime);
    mpz_mul(num, e1->num, e2->num);
    mpz_mod(num, num, prime);
    return S256Field_init(num);
}

S256Field* S256Field_s_mul(S256Field* e1, S256Field* e2) {
    mpz_t num;
    mpz_t n;
    mpz_init(num);
    mpz_init_set_str(n, N, 16);
    mpz_mul(num, e1->num, e2->num);
    mpz_mod(num, num, n);
    return S256Field_init(num);
}

S256Field* S256Field_mul_scalar(S256Field* e, mpz_t s) {
    mpz_t num;
    mpz_t prime;
    mpz_init(num);
    mpz_init_set(prime, e->prime);
    mpz_mul(num, e->num, s);
    mpz_mod(num, num, prime);
    return S256Field_init(num);
}

// Add a function to calculate the modular inverse
S256Field* S256Field_mod_inv(S256Field* e) {
    mpz_t base;
    mpz_t prime;
    mpz_t exponent;
    mpz_t result;
    mpz_init_set(base, e->num);
    mpz_init_set(prime, e->prime);
    mpz_init(exponent);
    mpz_sub_ui(exponent, prime, 2); //Fermat's Little Theorem
    mpz_init_set_ui(result, 1);

    while (mpz_cmp_ui(exponent, 0) > 0) {
        mpz_t temp;
        mpz_init(temp);
        mpz_mod_ui(temp, exponent, 2);
        if (mpz_cmp_ui(temp, 1) == 0) {
            mpz_mul(result, result, base);
            mpz_mod(result, result, prime);
        }
        mpz_pow_ui(base, base, 2);
        mpz_mod(base, base, prime);
        mpz_div_ui(exponent, exponent, 2);
        mpz_clear(temp);
    }
    mpz_clear(exponent);
    return S256Field_init(result);
}

S256Field* S256Field_s_inv(S256Field* e) {
    mpz_t base;
    mpz_t n;
    mpz_t exponent;
    mpz_t result;
    mpz_init_set(base, e->num);
    mpz_init_set_str(n, N, 16);
    mpz_init(exponent);
    mpz_sub_ui(exponent, n, 2); //Fermat's Little Theorem
    mpz_init_set_ui(result, 1);

    while (mpz_cmp_ui(exponent, 0) > 0) {
        mpz_t temp;
        mpz_init(temp);
        mpz_mod_ui(temp, exponent, 2);
        if (mpz_cmp_ui(temp, 1) == 0) {
            mpz_mul(result, result, base);
            mpz_mod(result, result, n);
        }
        mpz_pow_ui(base, base, 2);
        mpz_mod(base, base, n);
        mpz_div_ui(exponent, exponent, 2);
        mpz_clear(temp);
    }
    mpz_clear(exponent);
    return S256Field_init(result);
}

S256Field* S256Field_pow(S256Field* e, mpz_t exponent) {
    if (mpz_cmp_ui(exponent, 0) < 0) {
        // Calculate the modular inverse for negative exponents
        S256Field* inv = S256Field_mod_inv(e);
        // Use the positive equivalent of the exponent
        mpz_t positiveExponent;
        mpz_init(positiveExponent);
        mpz_mul_si(positiveExponent, exponent, -1);
        S256Field* result = S256Field_pow(inv, positiveExponent);
        S256Field_free(inv);
        return result;
    } else {
        mpz_t base;
        mpz_t prime;
        mpz_t result;
        mpz_t exp;
        mpz_t temp;
        mpz_init_set(base, e->num);
        mpz_init_set(prime, e->prime);
        mpz_init_set_ui(result, 1);
        mpz_init(exp);
        mpz_init(temp);
        mpz_sub_ui(temp, prime, 1); //Ensure exponent is within field size
        mpz_mod(exp, exponent, temp); //Ensure exponent is within field size

        while (mpz_cmp_ui(exp, 0) > 0) {
            mpz_mod_ui(temp, exp, 2);
            if (mpz_cmp_ui(temp, 1) == 0) {
                mpz_mul(result, result, base);
                mpz_mod(result, result, prime);
            }
            mpz_pow_ui(base, base, 2);
            mpz_mod(base, base, prime);
            mpz_div_ui(exp, exp, 2);
        }
        mpz_clear(temp);
        mpz_clear(exp);
        return S256Field_init(result);
    }
}

S256Field* S256Field_div(S256Field* e1, S256Field* e2) {
    S256Field* inv_e2 = S256Field_mod_inv(e2); //Correctly calculate the modular inverse of e2
    mpz_t num;
    mpz_t prime;
    mpz_init(num);
    mpz_init_set(prime, e1->prime);
    mpz_mul(num, e1->num, inv_e2->num);
    mpz_mod(num, num, prime);  //Then multiply e1->num by this inverse, modulo prime
    S256Field_free(inv_e2); // Clean up the temporary S256Field created for the inverse
    return S256Field_init(num);
}