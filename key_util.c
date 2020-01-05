#include "key_util.h"

int is_shift_key(__u16 code) {
    if (code == KEY_LEFTSHIFT ||
        code == KEY_RIGHTSHIFT) {
            return 1;
        }
    return 0;
}

void update_shift_register(__s32 value, __u_char shift_register) {
    if (value == KEY_PRESSED) {
        shift_register = 1;
    }
    if (value == KEY_RELEASE) {
        shift_register = 0;
    }
}
