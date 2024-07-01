#include "generateAddress.h"
#include "lib/all_libs.h"
#include "config.h"
#include "globals.h"
#include "bitcoin/ecc/privatekey.h"

enum generate_address_state {
    STATE_GENERATE_ADDRESS_WAITING,
    STATE_GENERATE_ADDRESS_INSTRUCTIONS,
    STATE_GENERATE_ADDRESS_CONFIRM,
    STATE_GENERATE_ADDRESS_COMPUTE,
    STATE_GENERATE_ADDRESS_DISPLAY
} generate_address_state;

static char secret[100];

static void display_instructions() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the secret phrase for your private key.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%s", secret);
}

static void display_confirm() {
    display_draw_string(STARTING_X, STARTING_Y,(const char*)secret, DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + 50, "Press the center button to generate the address.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_compute() {
    mpz_t secret_num;
    hash_to_mpz_t((const uint8_t*)secret, 6, secret_num);
    PrivateKey* key = PrivateKey_init(secret_num);
    uint8_t address[1024];
    S256Point_address(key->point, address, false, false);
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
            generate_address_state = STATE_GENERATE_ADDRESS_CONFIRM;
            display_clear(BACKGROUND_COLOR);
            break;
        case STATE_GENERATE_ADDRESS_CONFIRM:
            if (button_center() == 0) {
                generate_address_state = STATE_GENERATE_ADDRESS_COMPUTE;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case STATE_GENERATE_ADDRESS_COMPUTE:
            generate_address_state = STATE_GENERATE_ADDRESS_DISPLAY;
            break;
        case STATE_GENERATE_ADDRESS_DISPLAY:
            if (button_left() == 0) {
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