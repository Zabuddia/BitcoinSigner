#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "point.h"

Point* Point_init(FieldElement* x, FieldElement* y, FieldElement* a, FieldElement* b) {
    if (x == NULL && y == NULL) {
        return NULL; //The point at infinity
    }

    //y^2 != x^3 + a * x + b
    // FieldElement* y_squared = FieldElement_pow(y, 2);
    // FieldElement* x_cubed = FieldElement_pow(x, 3);
    // FieldElement* a_times_x = FieldElement_mul(a, x);
    // FieldElement* x_cubed_plus_a_times_x = FieldElement_add(x_cubed, a_times_x);
    // FieldElement* x_cubed_plus_a_times_x_plus_b = FieldElement_add(x_cubed_plus_a_times_x, b);

    // if (FieldElement_ne(y_squared, x_cubed_plus_a_times_x_plus_b)) {
    //     printf("Error: points are not on the curve:\n");
    //     FieldElement_toString(x);
    //     FieldElement_toString(y);
    //     exit(1);
    // }

    // FieldElement_free(y_squared);
    // FieldElement_free(x_cubed);
    // FieldElement_free(a_times_x);
    // FieldElement_free(x_cubed_plus_a_times_x);
    // FieldElement_free(x_cubed_plus_a_times_x_plus_b);

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
    free(p);
}

void Point_toString(Point* p) {
    if (p->x == NULL) {
        printf("Point(infinity)\n");
    } else {
        printf("Point(%d, %d)_%d_%d FieldElement(%d)\n", p->x->num, p->y->num, p->a->num, p->b->num, p->x->prime);
    }
}

int Point_eq(Point* p1, Point* p2) {
    return (FieldElement_eq(p1->x, p2->x) && FieldElement_eq(p1->y, p2->y) && FieldElement_eq(p1->a, p2->a) && FieldElement_eq(p1->b, p2->b));
}

int Point_ne(Point* p1, Point* p2) {
    return (FieldElement_ne(p1->x, p2->x) && FieldElement_ne(p1->y, p2->y) && FieldElement_ne(p1->a, p2->a) && FieldElement_ne(p1->b, p2->b));
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
        FieldElement* s_squared = FieldElement_pow(s, 2);
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
        return newPoint;
    } else {
        //s = (3*x1^2 + a)/(2*y1)
        //x3 = s^2 – 2*x1
        //y3 = s(x1 – x3) – y1
        FieldElement* x1 = p1->x;
        FieldElement* y1 = p1->y;
        FieldElement* a = p1->a;
        FieldElement* x1_squared = FieldElement_pow(x1, 2);
        FieldElement* three_times_x1_squared = FieldElement_mul_scalar(x1_squared, 3);
        FieldElement* three_times_x1_squared_plus_a = FieldElement_add(three_times_x1_squared, a);
        FieldElement* two_times_y1 = FieldElement_mul_scalar(y1, 2);
        FieldElement* s = FieldElement_div(three_times_x1_squared_plus_a, two_times_y1);
        FieldElement* s_squared = FieldElement_pow(s, 2);
        FieldElement* two_times_x1 = FieldElement_mul_scalar(x1, 2);
        FieldElement* x3 = FieldElement_div(s_squared, two_times_x1);
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
        return newPoint;
    }
}

Point* Point_mul(Point* p, int coefficient) {
    int coef = coefficient;
    Point* current = p; // Current point to add
    Point* result = Point_init(NULL, NULL, p->a, p->b); // Point at infinity as the initial result

    while (coef > 0) {
        if (coef & 1) {
            // If the current bit of coef is 1, add 'current' to 'result'
            Point* temp = Point_add(result, current);
            // Free the previous 'result' if necessary to prevent memory leaks
            Point_free(result); 
            result = temp;
        }
        // Double 'current'
        Point* temp = Point_add(current, current);
        // Free the previous 'current' if necessary
        Point_free(current); 
        current = temp;

        coef >>= 1; // Right shift coef by 1
    }
    // Note: Depending on the implementation, you might not want to free 'current' here if it's used outside
    // Point_free(current); // Uncomment if 'current' is dynamically allocated and not used after this function

    return result;
}
