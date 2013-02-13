#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "keyboard.h"

static int tty_fd = -1;
static enum {RAW, CANONICAL} ttystate = CANONICAL;

static int keyb_fd = -1;

bool init_keyboard()
{
	if((tty_fd = open("/dev/tty", O_RDWR)) == -1) {
		fprintf(stderr, "Cannot open /dev/tty : %s\n", strerror(errno));
		return false;
	}

	struct termios buf;

	if(tcgetattr(tty_fd, &buf) < 0) {
		fprintf(stderr, "Cannot get the tty parameters : %s\n", strerror(errno));
		return false;
	}

	buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	buf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	buf.c_cflag &= ~(CSIZE | PARENB);
	buf.c_cflag |= CS8;
	buf.c_oflag &= ~(OPOST);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	if(tcsetattr(tty_fd, TCSAFLUSH, &buf) < 0) {
		return false;
	}

	ttystate = RAW;
	return true;
}

void destroy_keyboard()
{
	struct termios buf;

	buf.c_lflag |= (ECHO | ICANON | IEXTEN | ISIG);
	buf.c_iflag |= (BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	buf.c_cflag |= (CSIZE | PARENB);
	buf.c_cflag &= CS8;
	buf.c_oflag |= (OPOST);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	ttystate = CANONICAL;
	close(tty_fd);

	tty_fd = -1;
}

int get_keyboard_fd()
{
	return tty_fd;
}

void process_keyboard_event()
{
	char key;
	if(read(tty_fd, &key, 1) < 1) {
		return;
	}

	if(key == 'q') {
		exit(0);
	}
	/* TODO:
	 * - handle system-wide key combinations (alt-tab?)
	 * - otherwise send keypress/release to focused window
	 */
}
