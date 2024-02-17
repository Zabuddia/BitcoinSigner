#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "s256point.h"

mpz_t A;
mpz_t B;

void Initialize_a_and_b() {
    mpz_init_set_ui(A, 0);
    mpz_init_set_ui(B, 7);
}

S256Point* S256Point_init(S256Field* x, S256Field* y) {
    if (x == NULL && y == NULL) {
        return NULL; //The point at infinity
    }

    mpz_t two;
    mpz_t three;
    mpz_init_set_ui(two, 2);
    mpz_init_set_ui(three, 3);

    S256Field* temp_a = S256Field_init(A);
    S256Field* temp_b = S256Field_init(B);

    //y^2 != x^3 + a * x + b
    S256Field* y_squared = S256Field_pow(y, two);
    S256Field* x_cubed = S256Field_pow(x, three);
    S256Field* a_times_x = S256Field_mul(temp_a, x);
    S256Field* x_cubed_plus_a_times_x = S256Field_add(x_cubed, a_times_x);
    S256Field* x_cubed_plus_a_times_x_plus_b = S256Field_add(x_cubed_plus_a_times_x, temp_b);

    mpz_clear(two);
    mpz_clear(three);

    if (S256Field_ne(y_squared, x_cubed_plus_a_times_x_plus_b)) {
        printf("Error: points are not on the curve:\n");
        S256Field_toString(x);
        S256Field_toString(y);
        exit(1);
    }

    S256Field_free(y_squared);
    S256Field_free(x_cubed);
    S256Field_free(a_times_x);
    S256Field_free(x_cubed_plus_a_times_x);
    S256Field_free(x_cubed_plus_a_times_x_plus_b);

    S256Point* p = malloc(sizeof(S256Point));
    if (p == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    p->x = x;
    p->y = y;
    p->a = temp_a;
    p->b = temp_b;
    return p;
}

void S256Point_free(S256Point* p) {
    if (p != NULL) {
        S256Field_free(p->x);
        S256Field_free(p->y);
        S256Field_free(p->a);
        S256Field_free(p->b);
        free(p);
    }
}


void S256Point_toString(S256Point* p) {
    if (p == NULL) {
        printf("S256Point(infinity)\n");
    } else {
        gmp_printf("S256Point(%Zd, %Zd)_%Zd_%Zd\n", p->x->num, p->y->num, p->a->num, p->b->num);
    }
}

int S256Point_eq(S256Point* p1, S256Point* p2) {
    return (S256Field_eq(p1->x, p2->x) && S256Field_eq(p1->y, p2->y) && S256Field_eq(p1->a, p2->a) && S256Field_eq(p1->b, p2->b));
}

int S256Point_ne(S256Point* p1, S256Point* p2) {
    return (S256Field_ne(p1->x, p2->x) || S256Field_ne(p1->y, p2->y) || S256Field_ne(p1->a, p2->a) || S256Field_ne(p1->b, p2->b));
}

S256Point* S256Point_add(S256Point* p1, S256Point* p2) {
    if (p1 == NULL) return p2;
    if (p2 == NULL) return p1;

    if (S256Field_ne(p1->a, p2->a) || S256Field_ne(p1->b, p2->b)) {
        printf("Trying to add points that are not on the same curve\n");
        exit(1);
    }

    if (S256Point_eq(p1, p2) && p1->y->num == 0) {
        return S256Point_init(NULL, NULL);
    }

    if (S256Field_eq(p1->x, p2->x) && S256Field_ne(p1->y, p2->y)) {
        return S256Point_init(NULL, NULL);
    }

    if (S256Field_ne(p1->x, p2->x)) {
        mpz_t two;
        mpz_init_set_ui(two, 2);

        //s = (y2 – y1)/(x2 – x1)
        //x3 = s^2 – x1 – x2
        //y3 = s(x1 – x3) – y1
        S256Field* x1 = p1->x;
        S256Field* x2 = p2->x;
        S256Field* y1 = p1->y;
        S256Field* y2 = p2->y;
        S256Field* y2_minus_y1 = S256Field_sub(y2, y1);
        S256Field* x2_minus_x1 = S256Field_sub(x2, x1);
        S256Field* s = S256Field_div(y2_minus_y1, x2_minus_x1);
        S256Field* s_squared = S256Field_pow(s, two);
        S256Field* s_squared_minus_x1 = S256Field_sub(s_squared, x1);
        S256Field* x3 = S256Field_sub(s_squared_minus_x1, x2);
        S256Field* x1_minux_x3 = S256Field_sub(x1, x3);
        S256Field* s_times_x1_minus_x3 = S256Field_mul(s, x1_minux_x3);
        S256Field* y3 = S256Field_sub(s_times_x1_minus_x3, y1);
        S256Point* newS256Point = S256Point_init(x3, y3);
        S256Field_free(y2_minus_y1);
        S256Field_free(x2_minus_x1);
        S256Field_free(s);
        S256Field_free(s_squared);
        S256Field_free(s_squared_minus_x1);
        S256Field_free(x1_minux_x3);
        S256Field_free(s_times_x1_minus_x3);
        mpz_clear(two);
        return newS256Point;
    } else {
        mpz_t two;
        mpz_t three;
        mpz_init_set_ui(two, 2);
        mpz_init_set_ui(three, 3);

        //s = (3*x1^2 + a)/(2*y1)
        //x3 = s^2 – 2*x1
        //y3 = s(x1 – x3) – y1
        S256Field* x1 = p1->x;
        S256Field* y1 = p1->y;
        S256Field* a = p1->a;
        S256Field* x1_squared = S256Field_pow(x1, two);
        S256Field* three_times_x1_squared = S256Field_mul_scalar(x1_squared, three);
        S256Field* three_times_x1_squared_plus_a = S256Field_add(three_times_x1_squared, a);
        S256Field* two_times_y1 = S256Field_mul_scalar(y1, two);
        S256Field* s = S256Field_div(three_times_x1_squared_plus_a, two_times_y1);
        S256Field* s_squared = S256Field_pow(s, two);
        S256Field* two_times_x1 = S256Field_mul_scalar(x1, two);
        S256Field* x3 = S256Field_sub(s_squared, two_times_x1);
        S256Field* x1_minux_x3 = S256Field_sub(x1, x3);
        S256Field* s_times_x1_minus_x3 = S256Field_mul(s, x1_minux_x3);
        S256Field* y3 = S256Field_sub(s_times_x1_minus_x3, y1);
        S256Point* newS256Point = S256Point_init(x3, y3);
        S256Field_free(x1_squared);
        S256Field_free(three_times_x1_squared);
        S256Field_free(three_times_x1_squared_plus_a);
        S256Field_free(two_times_y1);
        S256Field_free(s);
        S256Field_free(s_squared);
        S256Field_free(two_times_x1);
        S256Field_free(x1_minux_x3);
        S256Field_free(s_times_x1_minus_x3);
        mpz_clear(two);
        mpz_clear(three);
        return newS256Point;
    }
}

S256Point* S256Point_mul(S256Point* p, mpz_t coefficient) {
    S256Point* result = S256Point_init(NULL, NULL);
    S256Point* current = p;
    mpz_t coef;
    mpz_t n;
    mpz_init_set_str(n, N, 16);
    mpz_init_set(coef, coefficient);
    mpz_mod(coef, coef, n);
while (mpz_cmp_ui(coef, 0) > 0) { // While coef is greater than 0
    if (mpz_tstbit(coef, 0) == 1) { // If the least significant bit of coef is 1
        result = S256Point_add(result, current);
    }
    current = S256Point_add(current, current);
    mpz_fdiv_q_2exp(coef, coef, 1); // Right shift coef by 1
}
mpz_clear(coef);

return result;
}

int S256Point_verify(S256Point* p, S256Field* z, Signature* sig) {
    mpz_t gx;
    mpz_t gy;
    mpz_init_set_str(gx, GX, 16);
    mpz_init_set_str(gy, GY, 16);

    S256Field* x = S256Field_init(gx);
    S256Field* y = S256Field_init(gy);

    S256Point* G = S256Point_init(x, y);

    S256Field* s_inv = S256Field_s_inv(sig->s);
    S256Field* u = S256Field_s_mul(z, s_inv);
    S256Field* v = S256Field_s_mul(sig->r, s_inv);
    S256Point* u_times_G = S256Point_mul(G, u->num);
    S256Point* v_times_p = S256Point_mul(p, v->num);
    S256Point* total = S256Point_add(u_times_G, v_times_p);

    int isVerified = S256Field_eq(total->x, sig->r);
    
    S256Field_free(x);
    S256Field_free(y);
    S256Field_free(s_inv);
    S256Field_free(u);
    S256Field_free(v);
    S256Point_free(G);
    S256Point_free(u_times_G);
    S256Point_free(v_times_p);
    S256Point_free(total);

    return isVerified;
}