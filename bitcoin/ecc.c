#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ecc.h"

FieldElement* FieldElement_init(int num, int prime) {
    assert(num >= 0 && num < prime); //Check that num is between 0 and prime - 1
    FieldElement* element = (FieldElement*)malloc(sizeof(FieldElement));
    if (element == NULL) {
        exit(1); //Exit if memory allocation fails
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

int FieldElement_eq(FieldElement* self, FieldElement* other) {
    if (other == NULL) {
        return 0;
    }

    return self->num == other->num && self->prime == other->prime;
}

int FieldElement_ne(FieldElement* self, FieldElement* other) {
    if (other == NULL) {
        return 0;
    }

    return self->num != other->num || self->prime != other->prime;
}

FieldElement* FieldElement_add(FieldElement* self, FieldElement* other) {
    assert(self->prime == other->prime); //Make sure they are in the same field
    int num = (self->num + other->num) % self->prime;
    int prime = self->prime;
    return FieldElement_init(num, prime);
}

FieldElement* FieldElement_sub(FieldElement* self, FieldElement* other) {
    assert(self->prime == other->prime); //Make sure they are in the same field
    int num = (self->num - other->num + self->prime) % self->prime; //Ensure a positive result
    int prime = self->prime;
    return FieldElement_init(num, prime);
}

FieldElement* FieldElement_mul(FieldElement* self, FieldElement* other) {
    assert(self->prime == other->prime);
    int num = (self->num * other->num) % self->prime;
    int prime = self->prime;
    return FieldElement_init(num, prime);
}

// Add a function to calculate the modular inverse
FieldElement* FieldElement_mod_inv(FieldElement* self) {
    int exponent = self->prime - 2; // Fermat's Little Theorem
    int result = 1;
    int base = self->num;
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = (result * base) % self->prime;
        }
        base = (base * base) % self->prime;
        exponent /= 2;
    }
    return FieldElement_init(result, self->prime);
}

FieldElement* FieldElement_pow(FieldElement* self, int exponent) {
    if (exponent < 0) {
        // Calculate the modular inverse for negative exponents
        FieldElement* inv = FieldElement_mod_inv(self);
        // Use the positive equivalent of the exponent
        int positiveExponent = -exponent;
        FieldElement* result = FieldElement_pow(inv, positiveExponent);
        FieldElement_free(inv);
        return result;
    } else {
        int result = 1;
        int base = self->num;
        int exp = exponent % (self->prime - 1); // Ensure exponent is within field size
        while (exp > 0) {
            if (exp % 2 == 1) {
                result = (result * base) % self->prime;
            }
            base = (base * base) % self->prime;
            exp /= 2;
        }
        return FieldElement_init(result, self->prime);
    }
}

FieldElement* FieldElement_div(FieldElement* self, FieldElement* other) {
    assert(self->prime == other->prime);
    int inv = 1;
    int p_minus_2 = self->prime - 2;
    for (int i = 0; i < p_minus_2; i++) {
        inv = (inv * other->num) % self->prime;
    }
    int num = (self->num * inv) % self->prime;
    int prime = self->prime;
    return FieldElement_init(num, prime);
}