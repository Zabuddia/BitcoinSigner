#include "globals.h"
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <ctype.h>

#define KEYBOARD_DEVICE "/dev/input/event3"
#define MAX_INPUT_LENGTH 100

bool in_generate_address;
bool in_check_balance;
bool in_send_transaction;

void keyboard_input(char* input) {
    int fd;
    struct libevdev *dev = NULL;
    struct input_event ev;
    uint8_t buffer_index = 0;

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
        if (rc == 0 && ev.type == EV_KEY && ev.value >= 0) {
            if (ev.value == 1) { // Key press
                printf("Key code: %d\n", ev.code); // Debugging statement

                if (ev.code == KEY_ENTER) {
                    input[buffer_index] = '\0'; // Null-terminate the string
                    break;
                } else if (ev.code == KEY_BACKSPACE) {
                    if (buffer_index > 0) {
                        buffer_index--;
                        input[buffer_index] = '\0';
                    }
                } else if (ev.code < MAX_INPUT_LENGTH && keycode_to_char[ev.code] != '\0') {
                    // Handle alphanumeric keys and space
                    if (buffer_index < sizeof(input) - 1) {
                        input[buffer_index++] = keycode_to_char[ev.code];
                        printf("Interpreted as: %c\n", input[buffer_index-1]); // Debugging statement
                    }
                }
            }
        }
        usleep(1000); // Prevent CPU overuse
    }

    libevdev_free(dev);
    close(fd);
}

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