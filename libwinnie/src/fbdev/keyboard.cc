/*
winnie - an experimental window system

Copyright (C) 2013 Eleni Maria Stea

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Author: Eleni Maria Stea <elene.mst@gmail.com>
*/

#ifdef WINNIE_FBDEV
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "keyboard.h"
#include "shalloc.h"
#include "window.h"
#include "winnie.h"
#include "wm.h"

struct Keyboard {
	int dev_fd;
	enum {RAW, CANONICAL} ttystate;
};

static Keyboard *keyboard;

bool init_keyboard()
{
	if(!(keyboard = (Keyboard*)sh_malloc(sizeof *keyboard))) {
		return false;
	}

	get_subsys()->keyboard_offset = (int)((char*)keyboard - (char*)get_pool());

	keyboard->ttystate = keyboard->CANONICAL;
	keyboard->dev_fd = -1;

	if((keyboard->dev_fd = open("/dev/tty", O_RDWR)) == -1) {
		fprintf(stderr, "Cannot open /dev/tty : %s\n", strerror(errno));
		return false;
	}

	struct termios buf;

	if(tcgetattr(keyboard->dev_fd, &buf) < 0) {
		fprintf(stderr, "Cannot get the tty parameters : %s\n", strerror(errno));
		return false;
	}

	buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	buf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	buf.c_cflag &= ~(CSIZE | PARENB);
	buf.c_cflag |= CS8;
	buf.c_oflag &= ~(OPOST);

	if(tcsetattr(keyboard->dev_fd, TCSAFLUSH, &buf) < 0) {
		return false;
	}

	keyboard->ttystate = keyboard->RAW;
	return true;
}

void destroy_keyboard()
{
	struct termios buf;

	if(tcgetattr(keyboard->dev_fd, &buf) < 0) {
		fprintf(stderr, "Cannot get the tty parameters : %s\n", strerror(errno));
	}

	buf.c_lflag |= (ECHO | ICANON | IEXTEN | ISIG);
	buf.c_iflag |= (BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	buf.c_cflag |= (CSIZE | PARENB);
	buf.c_cflag &= CS8;
	buf.c_oflag |= (OPOST);

	if(tcsetattr(keyboard->dev_fd, TCSAFLUSH, &buf) < 0) {
		fprintf(stderr, "Cannot set the tty parameters : %s\n", strerror(errno));
	}

	keyboard->ttystate = keyboard->CANONICAL;

	if(keyboard->dev_fd != -1) {
		close(keyboard->dev_fd);
		keyboard->dev_fd = -1;
	}

	sh_free(keyboard);
}

bool client_open_keyboard(void *smem_start, int offset)
{
	keyboard = (unsigned char*)smem_start + offset;
	return true;
}

void client_close_keyboard()
{
}

int get_keyboard_fd()
{
	return keyboard->dev_fd;
}

void process_keyboard_event()
{
	char key;
	if(read(keyboard->dev_fd, &key, 1) < 1) {
		return;
	}

	if(key == 'q') {
		exit(0);
	}

	Window *focused_win = wm->get_focused_window();
	if(focused_win) {
		KeyboardFuncType keyb_callback = focused_win->get_keyboard_callback();
		if(keyb_callback) {
			keyb_callback(focused_win, key, true); //TODO: true??
		}
	}

	/* TODO:
	 * - handle system-wide key combinations (alt-tab?)
	 * - otherwise send keypress/release to focused window
	 */
}
#endif // WINNIE_FBDEV
