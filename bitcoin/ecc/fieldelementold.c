#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "fieldelementold.h"

FieldElementOld* FieldElementOld_init(int num, int prime) {
    assert(num >= 0 && num < prime); //Check that num is between 0 and prime - 1
    FieldElementOld* element = (FieldElementOld*)malloc(sizeof(FieldElementOld));
    if (element == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    element->num = num;
    element->prime = prime;
    return element;
}

void FieldElementOld_free(FieldElementOld* element) {
    free(element);
}

void FieldElementOld_toString(FieldElementOld* element) {
    printf("FieldElementOld_%d(%d)\n", element->prime, element->num);
}

int FieldElementOld_eq(FieldElementOld* e1, FieldElementOld* e2) {
    if (e2 == NULL) {
        return 0;
    }

    return e1->num == e2->num && e1->prime == e2->prime;
}

int FieldElementOld_ne(FieldElementOld* e1, FieldElementOld* e2) {
    if (e2 == NULL) {
        return 0;
    }

    return e1->num != e2->num || e1->prime != e2->prime;
}

FieldElementOld* FieldElementOld_add(FieldElementOld* e1, FieldElementOld* e2) {
    assert(e1->prime == e2->prime); //Make sure they are in the same field
    int num = (e1->num + e2->num) % e1->prime;
    int prime = e1->prime;
    return FieldElementOld_init(num, prime);
}

FieldElementOld* FieldElementOld_sub(FieldElementOld* e1, FieldElementOld* e2) {
    int prime = e1->prime;
    assert(e1->prime == e2->prime); //Make sure they are in the same field
    int num = ((e1->num - e2->num) + prime) % prime; //Ensure a positive result
    return FieldElementOld_init(num, prime);
}

FieldElementOld* FieldElementOld_mul(FieldElementOld* e1, FieldElementOld* e2) {
    assert(e1->prime == e2->prime);
    int num = (e1->num * e2->num) % e1->prime;
    int prime = e1->prime;
    return FieldElementOld_init(num, prime);
}

FieldElementOld* FieldElementOld_mul_scalar(FieldElementOld* e, int s) {
    int num = (e->num * s) % e->prime;
    int prime = e->prime;
    return FieldElementOld_init(num, prime);
}

// Add a function to calculate the modular inverse
FieldElementOld* FieldElementOld_mod_inv(FieldElementOld* e1) {
    int exponent = e1->prime - 2; // Fermat's Little Theorem
    int result = 1;
    int base = e1->num;
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = (result * base) % e1->prime;
        }
        base = (base * base) % e1->prime;
        exponent /= 2;
    }
    return FieldElementOld_init(result, e1->prime);
}

FieldElementOld* FieldElementOld_pow(FieldElementOld* e, int exponent) {
    if (exponent < 0) {
        // Calculate the modular inverse for negative exponents
        FieldElementOld* inv = FieldElementOld_mod_inv(e);
        // Use the positive equivalent of the exponent
        int positiveExponent = -exponent;
        FieldElementOld* result = FieldElementOld_pow(inv, positiveExponent);
        FieldElementOld_free(inv);
        return result;
    } else {
        int result = 1;
        int base = e->num;
        int exp = exponent % (e->prime - 1); // Ensure exponent is within field size
        while (exp > 0) {
            if (exp % 2 == 1) {
                result = (result * base) % e->prime;
            }
            base = (base * base) % e->prime;
            exp /= 2;
        }
        int prime = e->prime;
        return FieldElementOld_init(result, prime);
    }
}

FieldElementOld* FieldElementOld_div(FieldElementOld* e1, FieldElementOld* e2) {
    assert(e1->prime == e2->prime);
    FieldElementOld* inv_e2 = FieldElementOld_mod_inv(e2); // Correctly calculate the modular inverse of e2
    int num = (e1->num * inv_e2->num) % e1->prime; // Then multiply e1->num by this inverse, modulo prime
    int prime = e1->prime;
    FieldElementOld_free(inv_e2); // Clean up the temporary FieldElementOld created for the inverse
    return FieldElementOld_init(num, prime);
}