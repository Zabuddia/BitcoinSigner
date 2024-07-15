#include "globals.h"

bool in_generate_address;
bool in_check_balance;
bool in_send_transaction;

bool left_button_pressed() {
    return button_left() == 0;
}

bool up_button_pressed() {
    return button_up() == 0;
}

bool down_button_pressed() {
    return button_down() == 0;
}

bool center_button_pressed() {
    return button_center() == 0;
}

bool right_button_pressed() {
    return button_right() == 0;
}

bool key1_button_pressed() {
    return button_key_1() == 0;
}

bool key2_button_pressed() {
    return button_key_2() == 0;
}

bool key3_button_pressed() {
    return button_key_3() == 0;
}