#include <signal.h>
#include <assert.h>

#include "lib/all_libs.h"
#include "bitcoin/ecc/point.h"
#include "bitcoin/ecc/fieldelement.h"
#include "bitcoin/ecc/s256field.h"
#include "bitcoin/ecc/s256point.h"
#include "bitcoin/ecc/signature.h"

int main() {
    // mpz_t gx;
    // mpz_t gy;
    // mpz_init_set_str(gx, GX, 16);
    // mpz_init_set_str(gy, GY, 16);
    // S256Field* x = S256Field_init(gx);
    // S256Field* y = S256Field_init(gy);
    // S256Point* G = S256Point_init(x, y);

    mpz_t ex;
    mpz_t why;
    mpz_init_set_str(ex, "887387e452b8eacc4acfde10d9aaf7f6d9a0f975aabb10d006e4da568744d06c", 16);
    mpz_init_set_str(why, "61de6d95231cd89026e286df3b6ae4a894a3378e393e93a0f45b666329a0ae34", 16);
    S256Field* X = S256Field_init(ex);
    S256Field* Y = S256Field_init(why);
    S256Point* point = S256Point_init(X, Y);

    //signature 1
    mpz_t zee;
    mpz_t are;
    mpz_t ess;
    mpz_init_set_str(zee, "ec208baa0fc1c19f708a9ca96fdeff3ac3f230bb4a7ba4aede4942ad003c0f60", 16);
    mpz_init_set_str(are, "ac8d1c87e51d0d441be8b3dd5b05c8795b48875dffe00b7ffcfac23010d3a395", 16);
    mpz_init_set_str(ess, "68342ceff8935ededd102dd876ffd6ba72d6a427a3edb13d26eb0781cb423c4", 16);
    S256Field* Z = S256Field_init(zee);
    S256Field* R = S256Field_init(are);
    S256Field* S = S256Field_init(ess);
    Signature* sig = Signature_init(R, S);

    printf("Verified: %d\n", S256Point_verify(point, Z, sig));

    // # signature 2
    // z = 0x7c076ff316692a3d7eb3c3bb0f8b1488cf72e1afcd929e29307032997a838a3d
    // r = 0xeff69ef2b1bd93a66ed5219add4fb51e11a840f404876325a1e8ffe0529a2c
    // s = 0xc7207fee197d27c618aea621406f6bf5ef6fca38681d82b2f06fddbdce6feab6

    // s_inv = pow(s, N-2, N)
    // u = z * s_inv % N
    // v = r * s_inv % N
    // print((u*G + v*point).x.num == r)

    return 0;
}

// /* DEFAULT COLORS */
// #define BACKGROUND_COLOR WHITE
// #define FONT_COLOR BLACK

// // Makes sure to deinitialize everything before program close
// void intHandler(int dummy) {
//     log_info("Exiting...");
//     display_exit();
//     exit(0);
// }

// int main(void) {

//     signal(SIGINT, intHandler);

//     // Initialize the hardware here
//     log_info("Starting...");
//     display_init();
//     buttons_init();
//     display_clear(BACKGROUND_COLOR);
//     /* place lines here*/
//     delay_ms(1000);

//     RubiksCube rubiks_cube;
//     resetcube(&rubiks_cube);
//     display_draw_rubiks_cube(rubiks_cube);
//     // Everything inside this loop will repeat until 'Ctrl-C' is pressed in the terminal.
//     while (true) {
//         delay_ms(200);

//         // // Example of how to interact with a button
//         // if (button_up() == 0) {
//         // // Do something upon detecting button press

//         //     while (button_up() == 0) {
//         //         // Do something while the button is pressed
//         //         delay_ms(1);
//         //     }
//         // }

//         /* Write code to fufill lab requirements here*/
//         // if (button_up() == 0) {
//         //     display_clear(BACKGROUND_COLOR);
//         //     display_draw_rectangle(0, 0, DISPLAY_WIDTH, 20, GREEN, true, 1);
//         //     display_draw_rectangle(0, DISPLAY_HEIGHT - 20, DISPLAY_WIDTH, DISPLAY_HEIGHT, RED,
//         //     true,
//         //                            1);
//         //     display_draw_line(51, 39, 51, 89, FONT_COLOR, 3);
//         //     display_draw_line(77, 39, 77, 89, FONT_COLOR, 3);
//         //     display_draw_line(39, 51, 89, 51, FONT_COLOR, 3);
//         //     display_draw_line(39, 77, 89, 77, FONT_COLOR, 3);
//         //     display_draw_circle(64, 64, 30, YELLOW, false, 3);
//         // }

//         // if (button_down() == 0) {
//         //     display_clear(RED);
//         //     display_draw_point(64, 64, ORANGE, 59);
//         //     display_draw_point(64, 64, YELLOW, 54);
//         //     display_draw_point(64, 64, GREEN, 49);
//         //     display_draw_point(64, 64, BLUE, 44);
//         //     display_draw_point(64, 64, PURPLE, 39);
//         //     display_draw_point(64, 64, RED, 34);
//         //     display_draw_point(64, 64, ORANGE, 29);
//         //     display_draw_point(64, 64, YELLOW, 24);
//         //     display_draw_point(64, 64, GREEN, 19);
//         //     display_draw_point(64, 64, BLUE, 14);
//         //     display_draw_point(64, 64, PURPLE, 9);
//         //     display_draw_point(64, 64, WHITE, 5);
//         //     display_draw_point(64, 64, BLACK, 1);
//         // }

//         // if (button_right() == 0) {
//         //     display_clear(BACKGROUND_COLOR);
//         //     display_draw_string(34, 44, "Hello world!", &Font8, BACKGROUND_COLOR, RED);
//         //     display_draw_string(34, 52, "Hello world!", &Font8, BACKGROUND_COLOR, ORANGE);
//         //     display_draw_string(34, 60, "Hello world!", &Font8, BACKGROUND_COLOR, YELLOW);
//         //     display_draw_string(34, 68, "Hello world!", &Font8, BACKGROUND_COLOR, GREEN);
//         //     display_draw_string(34, 76, "Hello world!", &Font8, BACKGROUND_COLOR, BLUE);
//         //     // display_draw_rectangle(10, 10, 40, 20, ROYAL_BLUE, true, 1);
//         //     // display_draw_rectangle(20, 20, 30, 70, ROYAL_BLUE, true, 1);
//         //     // display_draw_rectangle(10, 70, 80, 80, ROYAL_BLUE, true, 1);
//         //     // display_draw_rectangle(80, 55, 90, 80, ROYAL_BLUE, true, 1);
//         //     // display_draw_rectangle(20, 100, 47, 110, ROYAL_BLUE, true, 1);
//         //     // display_draw_rectangle(63, 100, 90, 110, ROYAL_BLUE, true, 1);
//         //     // display_draw_line(55, 35, 33, 105, ROYAL_BLUE, 5);
//         //     // display_draw_line(55, 35, 77, 105, ROYAL_BLUE, 5);
//         //     // display_draw_char(88, 20, ')', &Font24, BACKGROUND_COLOR, RED);
//         //     // display_draw_char(97, 20, '(', &Font24, BACKGROUND_COLOR, RED);
//         //     // display_draw_circle(100, 30, 13, BLACK, false, 1);
//         // }

//         // if (button_left() == 0) {
//         //     display_clear(BACKGROUND_COLOR);
//         //     display_draw_char(1, 1, 'A', &Font24, BACKGROUND_COLOR, PURPLE);
//         //     display_draw_char(25, 1, 'l', &Font24, BACKGROUND_COLOR, PURPLE);
//         //     display_draw_char(49, 1, 'a', &Font24, BACKGROUND_COLOR, PURPLE);
//         //     display_draw_char(73, 1, 'n', &Font24, BACKGROUND_COLOR, PURPLE);
//         //     display_draw_char(97, 1, ' ', &Font24, BACKGROUND_COLOR, BACKGROUND_COLOR);
//         //     display_draw_char(1, 25, 'F', &Font24, BACKGROUND_COLOR, GOLD);
//         //     display_draw_char(25, 25, 'i', &Font24, BACKGROUND_COLOR, GOLD);
//         //     display_draw_char(49, 25, 'f', &Font24, BACKGROUND_COLOR, GOLD);
//         //     display_draw_char(73, 25, 'e', &Font24, BACKGROUND_COLOR, GOLD);
//         //     display_draw_char(97, 25, '!', &Font24, BACKGROUND_COLOR, RED);
//         //     display_draw_string(1, 49, "Pi: ", &Font16, BACKGROUND_COLOR, FONT_COLOR);
//         //     display_draw_number(40, 50, 3, &Font12, BACKGROUND_COLOR, FONT_COLOR);
//         //     display_draw_char(47, 50, '.', &Font12, BACKGROUND_COLOR, FONT_COLOR);
//         //     display_draw_number(54, 50, 141592653, &Font12, BACKGROUND_COLOR, FONT_COLOR);
//         // }

//         // if (button_center() == 0) {
//         //     display_clear(BACKGROUND_COLOR);
//         //     display_draw_image("./pic/byu_og.bmp");
//         // }

//         if (button_up() == 0) {
//             X(&rubiks_cube);
//             display_draw_rubiks_cube(rubiks_cube);
//         }

//         if (button_down() == 0) {
//             Xi(&rubiks_cube);
//             display_draw_rubiks_cube(rubiks_cube);
//         }

//         if (button_right() == 0) {
//             Yi(&rubiks_cube);
//             display_draw_rubiks_cube(rubiks_cube);
//         }

//         if (button_left() == 0) {
//             Y(&rubiks_cube);
//             display_draw_rubiks_cube(rubiks_cube);
//         }

//         // if (button_center() == 0) {
//         //     char move;
//         //     while (move != 'q') {
//         //         move = getmove();
//         //         makemove(&rubiks_cube, move);
//         //         display_draw_rubiks_cube(rubiks_cube);
//         //     }
//         // }

//         if (button_center() == 0) {
//             display_clear(BACKGROUND_COLOR);
//             display_draw_string(5, 5,
//                                 "Are you sure you want to quit? (press up for yes and down for no)",
//                                 &Font12, BACKGROUND_COLOR, FONT_COLOR);
//             delay_ms(1000);
//             while (button_down() != 0) {
//                 if (button_up() == 0) {
//                     display_clear(BACKGROUND_COLOR);
//                     exit(0);
//                 }
//             }
//             display_draw_rubiks_cube(rubiks_cube);
//         }

//         if (button_key_1() == 0) {
//             resetcube(&rubiks_cube);
//             display_draw_rubiks_cube(rubiks_cube);
//         }

//         // if (button_key_2() == 0) {
//         //     char *scramble_letters = getscramble();
//         //     scramble(&rubiks_cube, scramble_letters);
//         //     display_draw_rubiks_cube(rubiks_cube);
//         //     free(scramble_letters);
//         // }

//         if (button_key_2() == 0) {
//             display_clear(BACKGROUND_COLOR);
//             display_draw_rectangle(10, 10, 40, 20, ROYAL_BLUE, true, 1);
//             display_draw_rectangle(20, 20, 30, 70, ROYAL_BLUE, true, 1);
//             display_draw_rectangle(10, 70, 80, 80, ROYAL_BLUE, true, 1);
//             display_draw_rectangle(80, 55, 90, 80, ROYAL_BLUE, true, 1);
//             display_draw_rectangle(20, 100, 47, 110, ROYAL_BLUE, true, 1);
//             display_draw_rectangle(63, 100, 90, 110, ROYAL_BLUE, true, 1);
//             display_draw_line(55, 35, 33, 105, ROYAL_BLUE, 5);
//             display_draw_line(55, 35, 77, 105, ROYAL_BLUE, 5);
//             display_draw_char(88, 20, ')', &Font24, BACKGROUND_COLOR, RED);
//             display_draw_char(97, 20, '(', &Font24, BACKGROUND_COLOR, RED);
//             display_draw_circle(100, 30, 13, BLACK, false, 1);
//             delay_ms(1000);
//             display_draw_rubiks_cube(rubiks_cube);
//             while (button_key_2() != 0) {
//                 delay_ms(100);
//                 if (button_up() == 0) {
//                     R(&rubiks_cube);
//                     display_draw_rubiks_cube(rubiks_cube);
//                 }
//                 if (button_down() == 0) {
//                     Ri(&rubiks_cube);
//                     display_draw_rubiks_cube(rubiks_cube);
//                 }
//                 if (button_left() == 0) {
//                     U(&rubiks_cube);
//                     display_draw_rubiks_cube(rubiks_cube);
//                 }
//                 if (button_right() == 0) {
//                     Ui(&rubiks_cube);
//                     display_draw_rubiks_cube(rubiks_cube);
//                 }
//                 if (button_center() == 0) {
//                     Mi(&rubiks_cube);
//                     display_draw_rubiks_cube(rubiks_cube);
//                 }
//                 if (button_key_1() == 0) {
//                     Fi(&rubiks_cube);
//                     display_draw_rubiks_cube(rubiks_cube);
//                 }
//                 if (button_key_3() == 0) {
//                     F(&rubiks_cube);
//                     display_draw_rubiks_cube(rubiks_cube);
//                 }
//             }
//         }

//         // if (button_key_3() == 0) {
//         //     char *algorithm_string = getalgorithm();
//         //     performalgorithm(&rubiks_cube, algorithm_string);
//         //     display_draw_rubiks_cube(rubiks_cube);
//         //     free(algorithm_string);
//         // }

//         if (button_key_3() == 0) {
//             display_clear(BACKGROUND_COLOR);
//             display_draw_rectangle(10, 10, 40, 20, ROYAL_BLUE, true, 1);
//             display_draw_rectangle(20, 20, 30, 70, ROYAL_BLUE, true, 1);
//             display_draw_rectangle(10, 70, 80, 80, ROYAL_BLUE, true, 1);
//             display_draw_rectangle(80, 55, 90, 80, ROYAL_BLUE, true, 1);
//             display_draw_rectangle(20, 100, 47, 110, ROYAL_BLUE, true, 1);
//             display_draw_rectangle(63, 100, 90, 110, ROYAL_BLUE, true, 1);
//             display_draw_line(55, 35, 33, 105, ROYAL_BLUE, 5);
//             display_draw_line(55, 35, 77, 105, ROYAL_BLUE, 5);
//             display_draw_char(88, 20, ')', &Font24, BACKGROUND_COLOR, RED);
//             display_draw_char(97, 20, '(', &Font24, BACKGROUND_COLOR, RED);
//             display_draw_circle(100, 30, 13, BLACK, false, 1);
//         }
//     }
//     return 0;
// }
