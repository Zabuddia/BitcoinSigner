#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "fieldelement.h"

FieldElement* FieldElement_init(int num, int prime) {
    assert(num >= 0 && num < prime); //Check that num is between 0 and prime - 1
    FieldElement* element = (FieldElement*)malloc(sizeof(FieldElement));
    if (element == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    element->num = num;
    element->prime = prime;
    return element;
}

void FieldElement_free(FieldElement* element) {
    free(element);
}

void FieldElement_toString(FieldElement* element) {
    printf("FieldElement_%d(%d)\n", element->prime, element->num);
}

int FieldElement_eq(FieldElement* e1, FieldElement* e2) {
    if (e2 == NULL) {
        return 0;
    }

    return e1->num == e2->num && e1->prime == e2->prime;
}

int FieldElement_ne(FieldElement* e1, FieldElement* e2) {
    if (e2 == NULL) {
        return 0;
    }

    return e1->num != e2->num || e1->prime != e2->prime;
}

FieldElement* FieldElement_add(FieldElement* e1, FieldElement* e2) {
    assert(e1->prime == e2->prime); //Make sure they are in the same field
    int num = (e1->num + e2->num) % e1->prime;
    int prime = e1->prime;
    return FieldElement_init(num, prime);
}

FieldElement* FieldElement_sub(FieldElement* e1, FieldElement* e2) {
    int prime = e1->prime;
    assert(e1->prime == e2->prime); //Make sure they are in the same field
    int num = ((e1->num - e2->num) + prime) % prime; //Ensure a positive result
    return FieldElement_init(num, prime);
}

FieldElement* FieldElement_mul(FieldElement* e1, FieldElement* e2) {
    assert(e1->prime == e2->prime);
    int num = (e1->num * e2->num) % e1->prime;
    int prime = e1->prime;
    return FieldElement_init(num, prime);
}

FieldElement* FieldElement_mul_scalar(FieldElement* e, int s) {
    int num = (e->num * s) % e->prime;
    int prime = e->prime;
    return FieldElement_init(num, prime);
}

// Add a function to calculate the modular inverse
FieldElement* FieldElement_mod_inv(FieldElement* e1) {
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
    return FieldElement_init(result, e1->prime);
}

FieldElement* FieldElement_pow(FieldElement* e, int exponent) {
    if (exponent < 0) {
        // Calculate the modular inverse for negative exponents
        FieldElement* inv = FieldElement_mod_inv(e);
        // Use the positive equivalent of the exponent
        int positiveExponent = -exponent;
        FieldElement* result = FieldElement_pow(inv, positiveExponent);
        FieldElement_free(inv);
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
        return FieldElement_init(result, e->prime);
    }
}

FieldElement* FieldElement_div(FieldElement* e1, FieldElement* e2) {
    assert(e1->prime == e2->prime);
    FieldElement* inv_e2 = FieldElement_mod_inv(e2); // Correctly calculate the modular inverse of e2
    int num = (e1->num * inv_e2->num) % e1->prime; // Then multiply e1->num by this inverse, modulo prime
    int prime = e1->prime;
    FieldElement_free(inv_e2); // Clean up the temporary FieldElement created for the inverse
    return FieldElement_init(num, prime);
}
