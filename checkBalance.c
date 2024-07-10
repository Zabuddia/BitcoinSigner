#include "checkBalance.h"
#include "lib/all_libs.h"
#include "globals.h"
#include "config.h"
#include "bitcoin/tx/tx.h"

enum check_balance_state {
    CHECK_BALANCE_WAITING,
    CHECK_BALANCE_GETMODE,
    CHECK_BALANCE_GETADDR,
    CHECK_BALANCE_GETKEY,
    CHECK_BALANCE_FETCHING,
    CHECK_BALANCE_DISPLAYING
} check_balance_state;

static char address[100];

static void display_getmode() {
    display_draw_string(STARTING_X, STARTING_Y, "Press KEY1 to enter private key and press KEY2 to enter address to check balance.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_getaddr() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the address to check the balance of.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%s", address);
}

static void display_getkey() {
    char private_key[100];
    display_draw_string(STARTING_X, STARTING_Y, "Enter the private key to check the balance of.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%s", private_key);
    mpz_t secret_num;
    hash_to_mpz_t((const uint8_t*)private_key, strlen(private_key), secret_num);
    PrivateKey* key = PrivateKey_init(secret_num);
    S256Point_address(key->point, (uint8_t*)address, true, false);
    PrivateKey_free(key);
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
                check_balance_state = CHECK_BALANCE_GETMODE;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case CHECK_BALANCE_GETMODE:
            if (button_key_1() == 0) {
                check_balance_state = CHECK_BALANCE_GETKEY;
                display_clear(BACKGROUND_COLOR);
            } else if (button_key_2() == 0) {
                check_balance_state = CHECK_BALANCE_GETADDR;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case CHECK_BALANCE_GETADDR:
            check_balance_state = CHECK_BALANCE_FETCHING;
            display_clear(BACKGROUND_COLOR);
            break;
        case CHECK_BALANCE_GETKEY:
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
        case CHECK_BALANCE_GETMODE:
            display_getmode();
            break;
        case CHECK_BALANCE_GETADDR:
            display_getaddr();
            break;
        case CHECK_BALANCE_GETKEY:
            display_getkey();
            break;
        case CHECK_BALANCE_FETCHING:
            display_fetching();
            break;
        case CHECK_BALANCE_DISPLAYING:
            break;
    }
}