#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gmp.h>

#include "fieldelement.h"

FieldElement* FieldElement_init(mpz_t num, mpz_t prime) {
    assert(mpz_cmp_ui(num, 0) >= 0 && mpz_cmp(num, prime) < 0); //Check that num is between 0 and prime - 1
    FieldElement* element = (FieldElement*)malloc(sizeof(FieldElement));
    if (element == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    mpz_init_set(element->num, num);
    mpz_init_set(element->prime, prime);
    return element;
}

void FieldElement_free(FieldElement* element) {
    mpz_clear(element->num);
    mpz_clear(element->prime);
    free(element);
}

void FieldElement_toString(FieldElement* element) {
    gmp_printf("FieldElement_%Zd(%Zd)\n", element->prime, element->num);
}

int FieldElement_eq(FieldElement* e1, FieldElement* e2) {
    if (e2 == NULL) {
        return 0;
    }

    if (mpz_cmp(e1->num, e2->num) == 0 && mpz_cmp(e1->num, e2->num) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int FieldElement_ne(FieldElement* e1, FieldElement* e2) {
    if (mpz_cmp(e1->num, e2->num) != 0 || mpz_cmp(e1->num, e2->num) != 0) {
        return 1;
    } else {
        return 0;
    }
}

FieldElement* FieldElement_add(FieldElement* e1, FieldElement* e2) {
    assert(mpz_cmp(e1->prime, e2->prime) == 0); //Make sure they are in the same field
    mpz_t num;
    mpz_t prime;
    mpz_init(num);
    mpz_init_set(prime, e1->prime);
    mpz_add(num, e1->num, e2->num);
    mpz_mod(num, num, prime);
    return FieldElement_init(num, prime);
}

FieldElement* FieldElement_sub(FieldElement* e1, FieldElement* e2) {
    assert(mpz_cmp(e1->prime, e2->prime) == 0); //Make sure they are in the same field
    mpz_t num;
    mpz_t prime;
    mpz_init(num);
    mpz_init_set(prime, e1->prime);
    mpz_sub(num, e1->num, e2->num);
    mpz_add(num, num, prime); //Ensure a positive result
    mpz_mod(num, num, prime);
    return FieldElement_init(num, prime);
}

FieldElement* FieldElement_mul(FieldElement* e1, FieldElement* e2) {
    assert(mpz_cmp(e1->prime, e2->prime) == 0); //Make sure they are in the same field
    mpz_t num;
    mpz_t prime;
    mpz_init(num);
    mpz_init_set(prime, e1->prime);
    mpz_mul(num, e1->num, e2->num);
    mpz_mod(num, num, prime);
    return FieldElement_init(num, prime);
}

FieldElement* FieldElement_mul_scalar(FieldElement* e, mpz_t s) {
    mpz_t num;
    mpz_t prime;
    mpz_init(num);
    mpz_init_set(prime, e->prime);
    mpz_mul(num, e->num, s);
    mpz_mod(num, num, prime);
    return FieldElement_init(num, prime);
}

// Add a function to calculate the modular inverse
FieldElement* FieldElement_mod_inv(FieldElement* e) {
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
    return FieldElement_init(result, prime);
}

FieldElement* FieldElement_pow(FieldElement* e, mpz_t exponent) {
    if (mpz_cmp_ui(exponent, 0) < 0) {
        // Calculate the modular inverse for negative exponents
        FieldElement* inv = FieldElement_mod_inv(e);
        // Use the positive equivalent of the exponent
        mpz_t positiveExponent;
        mpz_init(positiveExponent);
        mpz_mul_si(positiveExponent, exponent, -1);
        FieldElement* result = FieldElement_pow(inv, positiveExponent);
        FieldElement_free(inv);
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
        return FieldElement_init(result, prime);
    }
}

FieldElement* FieldElement_div(FieldElement* e1, FieldElement* e2) {
    assert(mpz_cmp(e1->prime, e2->prime) == 0); //Make sure they are in the same field
    FieldElement* inv_e2 = FieldElement_mod_inv(e2); //Correctly calculate the modular inverse of e2
    mpz_t num;
    mpz_t prime;
    mpz_init(num);
    mpz_init_set(prime, e1->prime);
    mpz_mul(num, e1->num, inv_e2->num);
    mpz_mod(num, num, prime);  //Then multiply e1->num by this inverse, modulo prime
    FieldElement_free(inv_e2); // Clean up the temporary FieldElement created for the inverse
    return FieldElement_init(num, prime);
}