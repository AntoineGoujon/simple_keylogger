/*
 * Is called in order to identify the keyboard automatically amongst all input files
 * listed in the INPUT_PATH
 */ 

/*
 * https://www.linuxjournal.com/article/6429
 * https://unix.stackexchange.com/questions/94322/is-it-possible-for-a-daemon-i-e-background-process-to-look-for-key-presses-fr
 */

/* Ideas for improvement: look for every keyboard looking input file */
/* for now return only one keyboard */

/*
 * Default lookup path for devices
 */
#define INPUT_PATH "/dev/input"

/*
 * Returns pathname to one keyboard file
 */
int get_keyboard_pathname(char *keyboard_pathname);

/*
* Returns file descriptor to keyboard
*/
int get_keyboard_fd();

/*
 * Debug function
 */
void get_device_infos(int fd_device);
