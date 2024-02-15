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
    int num = (self->num - other->num) % self->prime;
    int prime = self->prime;
    return FieldElement_init(num, prime);
}