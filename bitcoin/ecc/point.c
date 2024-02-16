#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "point.h"

Point* Point_init(FieldElement* x, FieldElement* y, FieldElement* a, FieldElement* b) {
    if (x == NULL && y == NULL) {
        return NULL; //The point at infinity
    }

    mpz_t two;
    mpz_t three;
    mpz_init_set_ui(two, 2);
    mpz_init_set_ui(three, 3);
    //y^2 != x^3 + a * x + b
    FieldElement* y_squared = FieldElement_pow(y, two);
    FieldElement* x_cubed = FieldElement_pow(x, three);
    FieldElement* a_times_x = FieldElement_mul(a, x);
    FieldElement* x_cubed_plus_a_times_x = FieldElement_add(x_cubed, a_times_x);
    FieldElement* x_cubed_plus_a_times_x_plus_b = FieldElement_add(x_cubed_plus_a_times_x, b);

    mpz_clear(two);
    mpz_clear(three);

    if (FieldElement_ne(y_squared, x_cubed_plus_a_times_x_plus_b)) {
        printf("Error: points are not on the curve:\n");
        FieldElement_toString(x);
        FieldElement_toString(y);
        exit(1);
    }

    FieldElement_free(y_squared);
    FieldElement_free(x_cubed);
    FieldElement_free(a_times_x);
    FieldElement_free(x_cubed_plus_a_times_x);
    FieldElement_free(x_cubed_plus_a_times_x_plus_b);

    Point* p = malloc(sizeof(Point));
    if (p == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    p->x = x;
    p->y = y;
    p->a = a;
    p->b = b;
    return p;
}

void Point_free(Point* p) {
    if (p != NULL) {
        FieldElement_free(p->x);
        FieldElement_free(p->y);
        FieldElement_free(p->a);
        FieldElement_free(p->b);
        free(p);
    }
}


void Point_toString(Point* p) {
    if (p == NULL) {
        printf("Point(infinity)\n");
    } else {
        gmp_printf("Point(%Zd, %Zd)_%Zd_%Zd FieldElement(%Zd)\n", p->x->num, p->y->num, p->a->num, p->b->num, p->x->prime);
    }
}

int Point_eq(Point* p1, Point* p2) {
    return (FieldElement_eq(p1->x, p2->x) && FieldElement_eq(p1->y, p2->y) && FieldElement_eq(p1->a, p2->a) && FieldElement_eq(p1->b, p2->b));
}

int Point_ne(Point* p1, Point* p2) {
    return (FieldElement_ne(p1->x, p2->x) || FieldElement_ne(p1->y, p2->y) || FieldElement_ne(p1->a, p2->a) || FieldElement_ne(p1->b, p2->b));
}

Point* Point_add(Point* p1, Point* p2) {
    if (p1 == NULL) return p2;
    if (p2 == NULL) return p1;

    if (FieldElement_ne(p1->a, p2->a) || FieldElement_ne(p1->b, p2->b)) {
        printf("Trying to add points that are not on the same curve\n");
        exit(1);
    }

    if (Point_eq(p1, p2) && p1->y->num == 0) {
        return Point_init(NULL, NULL, p1->a, p1->b);
    }

    if (FieldElement_eq(p1->x, p2->x) && FieldElement_ne(p1->y, p2->y)) {
        return Point_init(NULL, NULL, p1->a, p1->b);
    }

    if (FieldElement_ne(p1->x, p2->x)) {
        mpz_t two;
        mpz_init_set_ui(two, 2);

        //s = (y2 – y1)/(x2 – x1)
        //x3 = s^2 – x1 – x2
        //y3 = s(x1 – x3) – y1
        FieldElement* x1 = p1->x;
        FieldElement* x2 = p2->x;
        FieldElement* y1 = p1->y;
        FieldElement* y2 = p2->y;
        FieldElement* y2_minus_y1 = FieldElement_sub(y2, y1);
        FieldElement* x2_minus_x1 = FieldElement_sub(x2, x1);
        FieldElement* s = FieldElement_div(y2_minus_y1, x2_minus_x1);
        FieldElement* s_squared = FieldElement_pow(s, two);
        FieldElement* s_squared_minus_x1 = FieldElement_sub(s_squared, x1);
        FieldElement* x3 = FieldElement_sub(s_squared_minus_x1, x2);
        FieldElement* x1_minux_x3 = FieldElement_sub(x1, x3);
        FieldElement* s_times_x1_minus_x3 = FieldElement_mul(s, x1_minux_x3);
        FieldElement* y3 = FieldElement_sub(s_times_x1_minus_x3, y1);
        Point* newPoint = Point_init(x3, y3, p1->a, p1->b);
        FieldElement_free(y2_minus_y1);
        FieldElement_free(x2_minus_x1);
        FieldElement_free(s);
        FieldElement_free(s_squared);
        FieldElement_free(s_squared_minus_x1);
        FieldElement_free(x1_minux_x3);
        FieldElement_free(s_times_x1_minus_x3);
        mpz_clear(two);
        return newPoint;
    } else {
        mpz_t two;
        mpz_t three;
        mpz_init_set_ui(two, 2);
        mpz_init_set_ui(three, 3);

        //s = (3*x1^2 + a)/(2*y1)
        //x3 = s^2 – 2*x1
        //y3 = s(x1 – x3) – y1
        FieldElement* x1 = p1->x;
        FieldElement* y1 = p1->y;
        FieldElement* a = p1->a;
        FieldElement* x1_squared = FieldElement_pow(x1, two);
        FieldElement* three_times_x1_squared = FieldElement_mul_scalar(x1_squared, three);
        FieldElement* three_times_x1_squared_plus_a = FieldElement_add(three_times_x1_squared, a);
        FieldElement* two_times_y1 = FieldElement_mul_scalar(y1, two);
        FieldElement* s = FieldElement_div(three_times_x1_squared_plus_a, two_times_y1);
        FieldElement* s_squared = FieldElement_pow(s, two);
        FieldElement* two_times_x1 = FieldElement_mul_scalar(x1, two);
        FieldElement* x3 = FieldElement_sub(s_squared, two_times_x1);
        FieldElement* x1_minux_x3 = FieldElement_sub(x1, x3);
        FieldElement* s_times_x1_minus_x3 = FieldElement_mul(s, x1_minux_x3);
        FieldElement* y3 = FieldElement_sub(s_times_x1_minus_x3, y1);
        Point* newPoint = Point_init(x3, y3, p1->a, p1->b);
        FieldElement_free(x1_squared);
        FieldElement_free(three_times_x1_squared);
        FieldElement_free(three_times_x1_squared_plus_a);
        FieldElement_free(two_times_y1);
        FieldElement_free(s);
        FieldElement_free(s_squared);
        FieldElement_free(two_times_x1);
        FieldElement_free(x1_minux_x3);
        FieldElement_free(s_times_x1_minus_x3);
        mpz_clear(two);
        mpz_clear(three);
        return newPoint;
    }
}

Point* Point_mul(Point* p, mpz_t coefficient) {
    Point* result = Point_init(NULL, NULL, p->a, p->b);
    Point* current = p;
    mpz_t coef;
    mpz_init_set(coef, coefficient);
while (mpz_cmp_ui(coef, 0) > 0) { // While coef is greater than 0
    if (mpz_tstbit(coef, 0) == 1) { // If the least significant bit of coef is 1
        result = Point_add(result, current);
    }
    current = Point_add(current, current);
    mpz_fdiv_q_2exp(coef, coef, 1); // Right shift coef by 1
}
mpz_clear(coef);

return result;
}