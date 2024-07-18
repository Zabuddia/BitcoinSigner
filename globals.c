#include "globals.h"
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <ctype.h>
#include "config.h"

#define KEYBOARD_DEVICE "/dev/input/event3"
#define MAX_INPUT_LENGTH 100

bool in_generate_address;
bool in_check_balance;
bool in_send_transaction;

enum button previous_button;

void keyboard_input(char* input) {
    int fd;
    struct libevdev *dev = NULL;
    struct input_event ev;
    uint8_t buffer_index = 0;
    int shift_pressed = 0;

    // Keycode to character mapping (simplified)
    const char keycode_to_char[] = {
        [KEY_1] = '1', [KEY_2] = '2', [KEY_3] = '3', [KEY_4] = '4', [KEY_5] = '5',
        [KEY_6] = '6', [KEY_7] = '7', [KEY_8] = '8', [KEY_9] = '9', [KEY_0] = '0',
        [KEY_Q] = 'q', [KEY_W] = 'w', [KEY_E] = 'e', [KEY_R] = 'r', [KEY_T] = 't',
        [KEY_Y] = 'y', [KEY_U] = 'u', [KEY_I] = 'i', [KEY_O] = 'o', [KEY_P] = 'p',
        [KEY_A] = 'a', [KEY_S] = 's', [KEY_D] = 'd', [KEY_F] = 'f', [KEY_G] = 'g',
        [KEY_H] = 'h', [KEY_J] = 'j', [KEY_K] = 'k', [KEY_L] = 'l', [KEY_Z] = 'z',
        [KEY_X] = 'x', [KEY_C] = 'c', [KEY_V] = 'v', [KEY_B] = 'b', [KEY_N] = 'n',
        [KEY_M] = 'm', [KEY_SPACE] = ' '
    };

    fd = open(KEYBOARD_DEVICE, O_RDONLY|O_NONBLOCK);
    if (fd < 0) {
        perror("Unable to open device");
        return;
    }

    libevdev_new_from_fd(fd, &dev);

    while (1) {
        int rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc == 0 && ev.type == EV_KEY) {
            if (ev.value == 1) { // Key press
                if (ev.code == KEY_LEFTSHIFT || ev.code == KEY_RIGHTSHIFT) {
                    shift_pressed = 1;
                } else if (ev.code == KEY_ENTER) {
                    input[buffer_index] = '\0'; // Null-terminate the string
                    break;
                } else if (ev.code == KEY_BACKSPACE) {
                    if (buffer_index > 0) {
                        buffer_index--;
                        input[buffer_index] = '\0';
                    }
                } else if (ev.code < sizeof(keycode_to_char) && keycode_to_char[ev.code] != '\0') {
                    // Handle alphanumeric keys and space
                    if (buffer_index < MAX_INPUT_LENGTH - 1) {
                        char ch = keycode_to_char[ev.code];
                        if (shift_pressed && isalpha(ch)) {
                            ch = toupper(ch);
                        }
                        input[buffer_index++] = ch;
                    }
                }
            } else if (ev.value == 0) { // Key release
                if (ev.code == KEY_LEFTSHIFT || ev.code == KEY_RIGHTSHIFT) {
                    shift_pressed = 0;
                }
            }
        }
        usleep(1000); // Prevent CPU overuse
    }

    libevdev_free(dev);
    close(fd);
}

bool left_button_pressed() {
    bool pressed = button_left() == 0;
    if (pressed) {
        previous_button = BUTTON_LEFT;
    }
    return pressed;
}

bool up_button_pressed() {
    bool pressed = button_up() == 0;
    if (pressed) {
        previous_button = BUTTON_UP;
    }
    return pressed;
}

bool down_button_pressed() {
    bool pressed = button_down() == 0;
    if (pressed) {
        previous_button = BUTTON_DOWN;
    }
    return pressed;
}

bool center_button_pressed() {
    bool pressed = button_center() == 0;
    if (pressed) {
        previous_button = BUTTON_CENTER;
    }
    return pressed;
}

bool right_button_pressed() {
    bool pressed = button_right() == 0;
    if (pressed) {
        previous_button = BUTTON_RIGHT;
    }
    return pressed;
}

bool key1_button_pressed() {
    bool pressed = button_key_1() == 0;
    if (pressed) {
        previous_button = BUTTON_KEY_1;
    }
    return pressed;
}

bool key2_button_pressed() {
    bool pressed = button_key_2() == 0;
    if (pressed) {
        previous_button = BUTTON_KEY_2;
    }
    return pressed;
}

bool key3_button_pressed() {
    bool pressed = button_key_3() == 0;
    if (pressed) {
        previous_button = BUTTON_KEY_3;
    }
    return pressed;
}

bool selected() {
    return center_button_pressed() || right_button_pressed();
}

bool return_to_menu() {
    return key3_button_pressed();
}

bool move_up() {
    return up_button_pressed() && (previous_button != BUTTON_UP);
}

bool move_down() {
    return down_button_pressed() && (previous_button != BUTTON_DOWN);
}