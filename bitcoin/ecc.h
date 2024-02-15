#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    int num;
    int prime;
} FieldElement;

FieldElement* FieldElement_init(int num, int prime);

void FieldElement_free(FieldElement* element);

void FieldElement_toString(FieldElement* element);

int FieldElement_eq(FieldElement* self, FieldElement* other);

int FieldElement_ne(FieldElement* self, FieldElement* other);

int FieldElement_add(FieldElement* self, FieldElement* other);

int FieldElement_sub(FieldElement* self, FieldElement* other);