#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include "keyboard.h"

int has_key_events(int device_fd)
{
    unsigned long evbit = 0;
    ioctl(device_fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);
    return evbit & (1 << EV_KEY); 
}

int has_specific_key(int device_fd, unsigned int key)
{
  size_t nchar = KEY_MAX/8 + 1;
  unsigned char bits[nchar];
  ioctl(device_fd, EVIOCGBIT(EV_KEY, sizeof(bits)), &bits);
  return bits[key/8] & (1 << (key % 8));
}

/* check for bits KEY_0 - KEY9 and KEY_KP0 to KEY_KP9 */
int has_keyboard_keys(int device_fd)
{
    for (int key_offset = 0; key_offset < 9; key_offset++ ) {
        if (!has_specific_key(device_fd, KEY_1 + key_offset)) {
            return 0;
        }
    }
    return 1;
}

int is_keyboard(const char *d_name)
{
    int device_fd;
    char d_pathname[32];
    sprintf(d_pathname, "%s/%s", INPUT_PATH, d_name);

    int is_keyboard_b = 0;

    if ((device_fd = open(d_pathname, O_RDONLY)) < 0) {
        #ifdef DEBUG
        fprintf(stderr,
            "ERR %d:\n"
            "Unable to open `%s'\n"
            "%s\n",
            errno, d_pathname, strerror(errno)
        );
        #endif
        exit(errno);
    }

    if (has_key_events(device_fd)) {
        if (has_keyboard_keys(device_fd)) {
            is_keyboard_b = 1;
        }
    }
    close(device_fd);
    return is_keyboard_b;
}

/* goes throught all devices and return the first keyboard */
char *list_d()
{
    struct dirent *dp;
    DIR *dir;
    
    char *keyboard_d = NULL;

    dir = opendir(INPUT_PATH);
    if (dir == NULL) {
        return keyboard_d;
    }

    while ((dp = readdir(dir)) != NULL) {
        if (strncmp(dp->d_name, "event", 5) == 0) {
            if (is_keyboard(dp->d_name)) {
                keyboard_d = dp->d_name;
                break;
            }
        }
    }
    closedir(dir);
    return keyboard_d;
}

int get_keyboard_pathname(char *keyboard_pathname)
{
    char *keyboard_name;
    keyboard_name = list_d();
    if (keyboard_name == NULL) {
        return 0;
    }
    sprintf(keyboard_pathname, "%s/%s", INPUT_PATH, keyboard_name);
    return 1;
}

int get_keyboard_fd()
{
    int fd_device;
    char keyboard_pathname[32];

    if (get_keyboard_pathname(keyboard_pathname) == 0) {
        exit(EXIT_FAILURE);
    }
    if ((fd_device = open(keyboard_pathname, O_RDONLY)) < 0) {
        exit(EXIT_FAILURE);
    }
    return fd_device;
}

void get_device_infos(int fd_device)
{
    unsigned version;
    unsigned short id[4];
    char name[256] = "N/A";
    
    ioctl(fd_device, EVIOCGVERSION, &version);
    ioctl(fd_device, EVIOCGID, id); 
    ioctl(fd_device, EVIOCGNAME(sizeof(name)), name);

    fprintf(stderr,
        "Name      : %s\n"
        "Version   : %d.%d.%d\n"
        "ID        : Bus=%04x Vendor=%04x Product=%04x Version=%04x\n"
        "----------\n"
        ,
        name,

        version >> 16,
        (version >> 8) & 0xff,
        version & 0xff,

        id[ID_BUS],
        id[ID_VENDOR],
        id[ID_PRODUCT],
        id[ID_VERSION]
    );
}