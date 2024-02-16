#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gmp.h>

#include "fieldelement.h"

FieldElement* FieldElement_init(mpz_t num, mpz_t prime) {
    mpz_t z;
    mpz_init_set_ui(z, 0);
    assert(mpz_cmp(num, z) >= 0 && mpz_cmp(num, prime) < 0); //Check that num is between 0 and prime - 1
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
    mpz_add(num, e1->num, e2->num);
    mpz_mod(num, num, e1->prime);
    mpz_init_set(prime, e1->prime);
    return FieldElement_init(num, prime);
}

// FieldElement* FieldElement_sub(FieldElement* e1, FieldElement* e2) {
//     mpz_t prime = e1->prime;
//     assert(e1->prime == e2->prime); //Make sure they are in the same field
//     mpz_t num = ((e1->num - e2->num) + prime) % prime; //Ensure a positive result
//     return FieldElement_init(num, prime);
// }

// FieldElement* FieldElement_mul(FieldElement* e1, FieldElement* e2) {
//     assert(e1->prime == e2->prime);
//     mpz_t num = (e1->num * e2->num) % e1->prime;
//     mpz_t prime = e1->prime;
//     return FieldElement_init(num, prime);
// }

// FieldElement* FieldElement_mul_scalar(FieldElement* e, mpz_t s) {
//     mpz_t num = (e->num * s) % e->prime;
//     mpz_t prime = e->prime;
//     return FieldElement_init(num, prime);
// }

// // Add a function to calculate the modular inverse
// FieldElement* FieldElement_mod_inv(FieldElement* e1) {
//     mpz_t exponent = e1->prime - 2; // Fermat's Little Theorem
//     mpz_t result = 1;
//     mpz_t base = e1->num;
//     while (exponent > 0) {
//         if (exponent % 2 == 1) {
//             result = (result * base) % e1->prime;
//         }
//         base = (base * base) % e1->prime;
//         exponent /= 2;
//     }
//     return FieldElement_init(result, e1->prime);
// }

// FieldElement* FieldElement_pow(FieldElement* e, mpz_t exponent) {
//     if (exponent < 0) {
//         // Calculate the modular inverse for negative exponents
//         FieldElement* inv = FieldElement_mod_inv(e);
//         // Use the positive equivalent of the exponent
//         mpz_t positiveExponent = -exponent;
//         FieldElement* result = FieldElement_pow(inv, positiveExponent);
//         FieldElement_free(inv);
//         return result;
//     } else {
//         mpz_t result = 1;
//         mpz_t base = e->num;
//         mpz_t exp = exponent % (e->prime - 1); // Ensure exponent is within field size
//         while (exp > 0) {
//             if (exp % 2 == 1) {
//                 result = (result * base) % e->prime;
//             }
//             base = (base * base) % e->prime;
//             exp /= 2;
//         }
//         mpz_t prime = e->prime;
//         return FieldElement_init(result, prime);
//     }
// }

// FieldElement* FieldElement_div(FieldElement* e1, FieldElement* e2) {
//     assert(e1->prime == e2->prime);
//     FieldElement* inv_e2 = FieldElement_mod_inv(e2); // Correctly calculate the modular inverse of e2
//     mpz_t num = (e1->num * inv_e2->num) % e1->prime; // Then multiply e1->num by this inverse, modulo prime
//     mpz_t prime = e1->prime;
//     FieldElement_free(inv_e2); // Clean up the temporary FieldElement created for the inverse
//     return FieldElement_init(num, prime);
// }