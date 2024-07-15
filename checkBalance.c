#include "checkBalance.h"
#include "lib/all_libs.h"
#include "globals.h"
#include "config.h"
#include "bitcoin/tx/tx.h"

enum check_balance_state {
    CHECK_BALANCE_WAITING,
    CHECK_BALANCE_GETMODE_PRIV,
    CHECK_BALANCE_GETMODE_ADDR,
    CHECK_BALANCE_GETADDR,
    CHECK_BALANCE_GETKEY_COMPRESS_YES,
    CHECK_BALANCE_GETKEY_COMPRESS_NO,
    CHECK_BALANCE_GETKEY_TESTNET_YES,
    CHECK_BALANCE_GETKEY_TESTNET_NO,
    CHECK_BALANCE_GETKEY,
    CHECK_BALANCE_FETCHING,
    CHECK_BALANCE_DISPLAYING
} check_balance_state;

static char address[100];
static bool compressed;
static bool testnet;

static void display_getmode_priv() {
    display_draw_string(STARTING_X, STARTING_Y, "What do you have?", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_LINES, "Private Key", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_LINES * 2, "Address", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_getmode_addr() {
    display_draw_string(STARTING_X, STARTING_Y, "What do you have?", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_LINES, "Private Key", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_LINES * 2, "Address", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
}

static void display_getaddr() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the address to check the balance of:", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%s", address);
}

static void display_getkey_compressed_yes() {
    compressed = false;
    display_draw_string(STARTING_X, STARTING_Y, "Do you want compressed format?", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_LINES, "YES", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_LINES * 2, "NO", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_getkey_compressed_no() {
    compressed = true;
    display_draw_string(STARTING_X, STARTING_Y, "Do you want compressed format?", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_LINES, "YES", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_LINES * 2, "NO", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
}

static void display_getkey_testnet_yes() {
    testnet = false;
    display_draw_string(STARTING_X, STARTING_Y, "Do you want testnet?", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_LINES, "YES", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_LINES * 2, "NO", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_getkey_testnet_no() {
    testnet = true;
    display_draw_string(STARTING_X, STARTING_Y, "Do you want testnet?", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_LINES, "YES", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_LINES * 2, "NO", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
}

static void display_getkey() {
    char private_key[100];
    display_draw_string(STARTING_X, STARTING_Y, "Enter the private key to check the balance of:", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%s", private_key);
    mpz_t secret_num;
    hash_to_mpz_t((const uint8_t*)private_key, strlen(private_key), secret_num);
    PrivateKey* key = PrivateKey_init(secret_num);
    S256Point_address(key->point, (uint8_t*)address, compressed, testnet);
    PrivateKey_free(key);
}

static void display_fetching() {
    uint64_t balance = get_balance(address);
    char balance_str[100];
    sprintf(balance_str, "Balance: %lu Satoshis", balance);
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
                check_balance_state = CHECK_BALANCE_GETMODE_PRIV;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case CHECK_BALANCE_GETMODE_PRIV:
            if (center_button_pressed()) {
                check_balance_state = CHECK_BALANCE_GETKEY_COMPRESS_YES;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                check_balance_state = CHECK_BALANCE_GETMODE_ADDR;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case CHECK_BALANCE_GETMODE_ADDR:
            if (center_button_pressed()) {
                check_balance_state = CHECK_BALANCE_GETADDR;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                check_balance_state = CHECK_BALANCE_GETMODE_PRIV;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case CHECK_BALANCE_GETADDR:
            check_balance_state = CHECK_BALANCE_FETCHING;
            display_clear(BACKGROUND_COLOR);
            break;
        case CHECK_BALANCE_GETKEY_COMPRESS_YES:
            if (center_button_pressed()) {
                check_balance_state = CHECK_BALANCE_GETKEY_TESTNET_YES;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                check_balance_state = CHECK_BALANCE_GETKEY_COMPRESS_NO;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case CHECK_BALANCE_GETKEY_COMPRESS_NO:
            if (center_button_pressed()) {
                check_balance_state = CHECK_BALANCE_GETKEY_TESTNET_YES;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                check_balance_state = CHECK_BALANCE_GETKEY_COMPRESS_YES;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case CHECK_BALANCE_GETKEY_TESTNET_YES:
            if (center_button_pressed()) {
                check_balance_state = CHECK_BALANCE_FETCHING;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                check_balance_state = CHECK_BALANCE_GETKEY_TESTNET_NO;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case CHECK_BALANCE_GETKEY_TESTNET_NO:
            if (center_button_pressed()) {
                check_balance_state = CHECK_BALANCE_FETCHING;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                check_balance_state = CHECK_BALANCE_GETKEY_TESTNET_YES;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case CHECK_BALANCE_GETKEY:
            check_balance_state = CHECK_BALANCE_FETCHING;
            display_clear(BACKGROUND_COLOR);
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
        case CHECK_BALANCE_GETMODE_PRIV:
            display_getmode_priv();
            break;
        case CHECK_BALANCE_GETMODE_ADDR:
            display_getmode_addr();
            break;
        case CHECK_BALANCE_GETADDR:
            display_getaddr();
            break;
        case CHECK_BALANCE_GETKEY_COMPRESS_YES:
            display_getkey_compress_yes();
            break;
        case CHECK_BALANCE_GETKEY_COMPRESS_NO:
            display_getkey_compress_no();
            break;
        case CHECK_BALANCE_GETKEY_TESTNET_YES:
            display_getkey_testnet_yes();
            break;
        case CHECK_BALANCE_GETKEY_TESTNET_NO:
            display_getkey_testnet_no();
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