#include "checkBalance.h"
#include "lib/all_libs.h"
#include "globals.h"
#include "config.h"
#include "bitcoin/tx/tx.h"

enum check_balance_state {
    CHECK_BALANCE_WAITING,
    CHECK_BALANCE_GETADDR,
    CHECK_BALANCE_FETCHING,
    CHECK_BALANCE_DISPLAYING
} check_balance_state;

static char address[100];

static void display_getaddr() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the address to check the balance of.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%s", address);
}

static void display_fetching() {
    uint64_t balance = get_balance(address);
    char balance_str[100];
    sprintf(balance_str, "Balance: %lu", balance);
    display_draw_string(STARTING_X, STARTING_Y, balance_str, DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

void check_balance_init() {
    check_balance_state = CHECK_BALANCE_WAITING;
}

void check_balance_tick() {
    //Transitions
    switch (check_balance_state) {
        case CHECK_BALANCE_WAITING:
            if (in_check_balance) {
                check_balance_state = CHECK_BALANCE_GETADDR;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case CHECK_BALANCE_GETADDR:
            check_balance_state = CHECK_BALANCE_FETCHING;
            display_clear(BACKGROUND_COLOR);
            break;
        case CHECK_BALANCE_FETCHING:
            check_balance_state = CHECK_BALANCE_DISPLAYING;
            break;
        case CHECK_BALANCE_DISPLAYING:
            if (button_left() == 0) {
                check_balance_state = CHECK_BALANCE_WAITING;
            }
            break;
    }
    //Actions
    switch (check_balance_state) {
        case CHECK_BALANCE_WAITING:
            break;
        case CHECK_BALANCE_GETADDR:
            display_getaddr();
            break;
        case CHECK_BALANCE_FETCHING:
            display_fetching();
            break;
        case CHECK_BALANCE_DISPLAYING:
            break;
    }
}