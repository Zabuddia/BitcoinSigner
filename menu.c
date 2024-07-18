#include "menu.h"
#include "lib/all_libs.h"
#include "config.h"
#include "globals.h"

enum menu_state {
    STATE_WAITING,
    STATE_GENERATE_ADDRESS,
    STATE_CHECK_BALANCE,
    STATE_SEND_TRANSACTION,
    STATE_IN_GENERATE_ADDRESS,
    STATE_IN_CHECK_BALANCE,
    STATE_IN_SEND_TRANSACTION,
} menu_state;

static void draw_menu() {

    if (menu_state == STATE_GENERATE_ADDRESS) {
        display_draw_string(STARTING_X, STARTING_Y, "Make Address", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
    } else {
        display_draw_string(STARTING_X, STARTING_Y, "Make Address", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    }
    if (menu_state == STATE_CHECK_BALANCE) {
        display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "Check Balance", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
    } else {
        display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "Check Balance", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    }
    if (menu_state == STATE_SEND_TRANSACTION) {
        display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "Transaction", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
    } else {
        display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "Transaction", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    }
}

void menu_init() {
    menu_state = STATE_GENERATE_ADDRESS;
}

void menu_tick() {
    //Transitions
    switch (menu_state) {
        case STATE_WAITING:
            if (return_to_menu()) {
                menu_state = STATE_GENERATE_ADDRESS;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case STATE_GENERATE_ADDRESS:
            if (move_up()) {
                menu_state = STATE_SEND_TRANSACTION;
            } else if (move_down()) {
                menu_state = STATE_CHECK_BALANCE;
            } else if (selected()) {
                menu_state = STATE_IN_GENERATE_ADDRESS;
            }
            break;
        case STATE_CHECK_BALANCE:
            if (move_up()) {
                menu_state = STATE_GENERATE_ADDRESS;
            } else if (move_down()) {
                menu_state = STATE_SEND_TRANSACTION;
            } else if (selected()) {
                menu_state = STATE_IN_CHECK_BALANCE;
            }
            break;
        case STATE_SEND_TRANSACTION:
            if (move_up()) {
                menu_state = STATE_CHECK_BALANCE;
            } else if (move_down()) {
                menu_state = STATE_GENERATE_ADDRESS;
            } else if (selected()) {
                menu_state = STATE_IN_SEND_TRANSACTION;
            }
            break;
        case STATE_IN_GENERATE_ADDRESS:
            menu_state = STATE_WAITING;
            break;
        case STATE_IN_CHECK_BALANCE:
            menu_state = STATE_WAITING;
            break;
        case STATE_IN_SEND_TRANSACTION:
            menu_state = STATE_WAITING;
            break;
    }
    //Actions
    switch (menu_state) {
        case STATE_WAITING:
            in_generate_address = false;
            in_check_balance = false;
            in_send_transaction = false;
            break;
        case STATE_GENERATE_ADDRESS:
            draw_menu();
            break;
        case STATE_CHECK_BALANCE:
            draw_menu();
            break;
        case STATE_SEND_TRANSACTION:
            draw_menu();
            break;
        case STATE_IN_GENERATE_ADDRESS:
            in_generate_address = true;
            in_check_balance = false;
            in_send_transaction = false;
            break;
        case STATE_IN_CHECK_BALANCE:
            in_generate_address = false;
            in_check_balance = true;
            in_send_transaction = false;
            break;
        case STATE_IN_SEND_TRANSACTION:
            in_generate_address = false;
            in_check_balance = false;
            in_send_transaction = true;
            break;
    }
}