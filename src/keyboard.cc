#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "keyboard.h"
#include "window.h"
#include "wm.h"

static int dev_fd = -1;
static enum {RAW, CANONICAL} ttystate = CANONICAL;

bool init_keyboard()
{
	if((dev_fd = open("/dev/tty", O_RDWR)) == -1) {
		fprintf(stderr, "Cannot open /dev/tty : %s\n", strerror(errno));
		return false;
	}

	struct termios buf;

	if(tcgetattr(dev_fd, &buf) < 0) {
		fprintf(stderr, "Cannot get the tty parameters : %s\n", strerror(errno));
		return false;
	}

	buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	buf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	buf.c_cflag &= ~(CSIZE | PARENB);
	buf.c_cflag |= CS8;
	buf.c_oflag &= ~(OPOST);

	if(tcsetattr(dev_fd, TCSAFLUSH, &buf) < 0) {
		return false;
	}

	ttystate = RAW;
	return true;
}

void destroy_keyboard()
{
	struct termios buf;

	if(tcgetattr(dev_fd, &buf) < 0) {
		fprintf(stderr, "Cannot get the tty parameters : %s\n", strerror(errno));
	}

	buf.c_lflag |= (ECHO | ICANON | IEXTEN | ISIG);
	buf.c_iflag |= (BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	buf.c_cflag |= (CSIZE | PARENB);
	buf.c_cflag &= CS8;
	buf.c_oflag |= (OPOST);

	if(tcsetattr(dev_fd, TCSAFLUSH, &buf) < 0) {
		fprintf(stderr, "Cannot set the tty parameters : %s\n", strerror(errno));
	}

	ttystate = CANONICAL;
	close(dev_fd);

	dev_fd = -1;
}

int get_keyboard_fd()
{
	return dev_fd;
}

void process_keyboard_event()
{
	char key;
	if(read(dev_fd, &key, 1) < 1) {
		return;
	}

	if(key == 'q') {
		exit(0);
	}

	Window *focused_win = wm->get_focused_window();
	if(focused_win) {
		KeyboardFuncType keyb_callback = focused_win->get_keyboard_callback();
		if(keyb_callback) {
			keyb_callback(focused_win, key, true);
		}
	}

	/* TODO:
	 * - handle system-wide key combinations (alt-tab?)
	 * - otherwise send keypress/release to focused window
	 */
}
