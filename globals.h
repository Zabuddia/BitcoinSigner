#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stdint.h>

#include "lib/all_libs.h"

extern bool in_generate_address;
extern bool in_check_balance;
extern bool in_send_transaction;

extern bool hold;
extern uint8_t hold_ticks;

void keyboard_input(char* input);

bool left_button_pressed();
bool up_button_pressed();
bool down_button_pressed();
bool center_button_pressed();
bool right_button_pressed();
bool key1_button_pressed();
bool key2_button_pressed();
bool key3_button_pressed();
bool move_up();
bool move_down();
bool selected();
bool return_to_menu();
bool go_back();

#endif // GLOBALS_H