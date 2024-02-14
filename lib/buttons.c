#include <bcm2835.h>

#include "buttons.h"

#define KEY_UP_PIN 6
#define KEY_DOWN_PIN 19
#define KEY_LEFT_PIN 5
#define KEY_RIGHT_PIN 26
#define KEY_PRESS_PIN 13
#define KEY1_PIN 21
#define KEY2_PIN 20
#define KEY3_PIN 16

void buttons_init() {
    bcm2835_gpio_fsel(KEY_UP_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(KEY_UP_PIN, BCM2835_GPIO_PUD_UP);

    bcm2835_gpio_fsel(KEY_DOWN_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(KEY_DOWN_PIN, BCM2835_GPIO_PUD_UP);

    bcm2835_gpio_fsel(KEY_LEFT_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(KEY_LEFT_PIN, BCM2835_GPIO_PUD_UP);

    bcm2835_gpio_fsel(KEY_RIGHT_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(KEY_RIGHT_PIN, BCM2835_GPIO_PUD_UP);

    bcm2835_gpio_fsel(KEY_PRESS_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(KEY_PRESS_PIN, BCM2835_GPIO_PUD_UP);

    bcm2835_gpio_fsel(KEY1_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(KEY1_PIN, BCM2835_GPIO_PUD_UP);

    bcm2835_gpio_fsel(KEY2_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(KEY2_PIN, BCM2835_GPIO_PUD_UP);

    bcm2835_gpio_fsel(KEY3_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(KEY3_PIN, BCM2835_GPIO_PUD_UP);
}

uint8_t button_up() { return bcm2835_gpio_lev(KEY_RIGHT_PIN); }

uint8_t button_down() { return bcm2835_gpio_lev(KEY_LEFT_PIN); }

uint8_t button_left() { return bcm2835_gpio_lev(KEY_UP_PIN); }

uint8_t button_right() { return bcm2835_gpio_lev(KEY_DOWN_PIN); }

uint8_t button_center() { return bcm2835_gpio_lev(KEY_PRESS_PIN); }

uint8_t button_key_1() { return bcm2835_gpio_lev(KEY1_PIN); }

uint8_t button_key_2() { return bcm2835_gpio_lev(KEY2_PIN); }

uint8_t button_key_3() { return bcm2835_gpio_lev(KEY3_PIN); }
