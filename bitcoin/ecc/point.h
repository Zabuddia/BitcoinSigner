#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    FieldElement* x, y;
    FieldElement* a, b;
} Point;

Point* Point_init(FieldElement* x, FieldElement* y, FieldElement* a, FieldElement* b);

void Point_free(Point* p);

int Point_eq(Point* p1, Point* p2);

int Point_ne(Point* p1, Point* p2);

Point* Point_add(Point* p1, Point* p2);