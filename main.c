#include <signal.h>
#include <assert.h>

#include "lib/all_libs.h"
#include "bitcoin/ecc/point.h"
#include "bitcoin/ecc/fieldelement.h"

int main() {
    int prime = 223;
    FieldElement* a = FieldElement_init(0, prime);
    FieldElement* b = FieldElement_init(7, prime);
    FieldElement* x1 = FieldElement_init(15, prime);
    FieldElement* y1 = FieldElement_init(86, prime);
    FieldElement* x2 = FieldElement_init(17, prime);
    FieldElement* y2 = FieldElement_init(56, prime);

    Point* p1 = Point_init(x1, y1, a, b);
    Point* p2 = Point_init(x2, y2, a, b);

    Point* addedPoints = Point_mul(p1, 2);
    Point_toString(addedPoints);

    Point_free(p1);
    Point_free(p2);
    Point_free(addedPoints);

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
