#include "generateAddress.h"
#include "lib/all_libs.h"
#include "config.h"
#include "globals.h"
#include "bitcoin/ecc/privatekey.h"

enum generate_address_state {
    STATE_GENERATE_ADDRESS_WAITING,
    STATE_GENERATE_ADDRESS_INSTRUCTIONS,
    STATE_GENERATE_ADDRESS_COMPRESS_YES,
    STATE_GENERATE_ADDRESS_COMPRESS_NO,
    STATE_GENERATE_ADDRESS_TESTNET_YES,
    STATE_GENERATE_ADDRESS_TESTNET_NO,
    STATE_GENERATE_ADDRESS_CONFIRM,
    STATE_GENERATE_ADDRESS_COMPUTE,
    STATE_GENERATE_ADDRESS_DISPLAY
} generate_address_state;

static char secret[100];
static bool compressed;
static bool testnet;

static void display_instructions() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the secret phrase for your private key.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%s", secret);
}

static void display_compress_yes() {
    compressed = true;
    display_draw_string(STARTING_X, STARTING_Y, "Do you want compressed format?", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "YES", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "NO", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_compress_no() {
    compressed = false;
    display_draw_string(STARTING_X, STARTING_Y, "Do you want compressed format?", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "YES", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "NO", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
}

static void display_testnet_yes() {
    testnet = true;
    display_draw_string(STARTING_X, STARTING_Y, "Do you want testnet?", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "YES", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "NO", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_testnet_no() {
    testnet = false;
    display_draw_string(STARTING_X, STARTING_Y, "Do you want testnet?", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "YES", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "NO", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
}

static void display_confirm() {
    display_draw_string(STARTING_X, STARTING_Y,(const char*)secret, DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "Press the center button to generate the address.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_compute() {
    mpz_t secret_num;
    hash_to_mpz_t((const uint8_t*)secret, strlen(secret), secret_num);
    PrivateKey* key = PrivateKey_init(secret_num);
    uint8_t address[1024];
    S256Point_address(key->point, address, compressed, testnet);
    display_draw_string(STARTING_X, STARTING_X, (const char*)address, DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    PrivateKey_free(key);
}

void generate_address_init() {
    generate_address_state = STATE_GENERATE_ADDRESS_WAITING;
}

void generate_address_tick() {
    //Transitions
    switch (generate_address_state) {
        case STATE_GENERATE_ADDRESS_WAITING:
            if (in_generate_address) {
                generate_address_state = STATE_GENERATE_ADDRESS_INSTRUCTIONS;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case STATE_GENERATE_ADDRESS_INSTRUCTIONS:
            if (key3_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_WAITING;
                display_clear(BACKGROUND_COLOR);
            } else {
                generate_address_state = STATE_GENERATE_ADDRESS_COMPRESS_YES;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case STATE_GENERATE_ADDRESS_COMPRESS_YES:
            if (center_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_TESTNET_YES;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_COMPRESS_NO;
                display_clear(BACKGROUND_COLOR);
            } else if (key3_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_WAITING;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case STATE_GENERATE_ADDRESS_COMPRESS_NO:
            if (center_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_TESTNET_YES;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_COMPRESS_YES;
                display_clear(BACKGROUND_COLOR);
            } else if (key3_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_WAITING;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case STATE_GENERATE_ADDRESS_TESTNET_YES:
            if (center_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_CONFIRM;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_TESTNET_NO;
                display_clear(BACKGROUND_COLOR);
            } else if (key3_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_WAITING;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case STATE_GENERATE_ADDRESS_TESTNET_NO:
            if (center_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_CONFIRM;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_TESTNET_YES;
                display_clear(BACKGROUND_COLOR);
            } else if (key3_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_WAITING;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case STATE_GENERATE_ADDRESS_CONFIRM:
            if (center_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_COMPUTE;
                display_clear(BACKGROUND_COLOR);
            } else if (key3_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_WAITING;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case STATE_GENERATE_ADDRESS_COMPUTE:
            generate_address_state = STATE_GENERATE_ADDRESS_DISPLAY;
            break;
        case STATE_GENERATE_ADDRESS_DISPLAY:
            if (key3_button_pressed()) {
                generate_address_state = STATE_GENERATE_ADDRESS_WAITING;
            }
            break;
    }
    //Actions
    switch (generate_address_state) {
        case STATE_GENERATE_ADDRESS_WAITING:
            break;
        case STATE_GENERATE_ADDRESS_INSTRUCTIONS:
            display_instructions();
            break;
        case STATE_GENERATE_ADDRESS_COMPRESS_YES:
            display_compress_yes();
            break;
        case STATE_GENERATE_ADDRESS_COMPRESS_NO:
            display_compress_no();
            break;
        case STATE_GENERATE_ADDRESS_TESTNET_YES:
            display_testnet_yes();
            break;
        case STATE_GENERATE_ADDRESS_TESTNET_NO:
            display_testnet_no();
            break;
        case STATE_GENERATE_ADDRESS_CONFIRM:
            display_confirm();
            break;
        case STATE_GENERATE_ADDRESS_COMPUTE:
            display_compute();
            break;
        case STATE_GENERATE_ADDRESS_DISPLAY:
            break;
    }
}