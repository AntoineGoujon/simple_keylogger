#include <linux/input.h>

#define KEY_RELEASE 0
#define KEY_PRESSED 1
#define KEY_REPEATED 2


int is_shift_key(__u16 key_code);

void update_shift_register(__s32 value, __u_char shift_register);
