#include "globals.h"
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <ctype.h>

#define KEYBOARD_DEVICE "/dev/input/event3"

bool in_generate_address;
bool in_check_balance;
bool in_send_transaction;

void keyboard_input(char* input) {
    int fd;
    struct libevdev *dev = NULL;
    struct input_event ev;
    uint8_t buffer_index = 0;

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
                } else if (ev.code >= KEY_1 && ev.code <= KEY_9) {
                    // Handle numeric keys 1-9
                    if (buffer_index < sizeof(input) - 1) {
                        input[buffer_index++] = '0' + (ev.code - KEY_1 + 1);
                        printf("Interpreted as: %c\n", input[buffer_index-1]); // Debugging statement
                    }
                } else if (ev.code == KEY_0) {
                    // Handle numeric key 0
                    if (buffer_index < sizeof(input) - 1) {
                        input[buffer_index++] = '0';
                        printf("Interpreted as: %c\n", input[buffer_index-1]); // Debugging statement
                    }
                } else if (ev.code >= KEY_A && ev.code <= KEY_Z) {
                    // Handle alphabetic keys
                    if (buffer_index < sizeof(input) - 1) {
                        input[buffer_index++] = 'a' + (ev.code - KEY_A);
                        printf("Interpreted as: %c\n", input[buffer_index-1]); // Debugging statement
                    }
                } else if (ev.code == KEY_SPACE) {
                    // Handle space
                    if (buffer_index < sizeof(input) - 1) {
                        input[buffer_index++] = ' ';
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