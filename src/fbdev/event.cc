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
#include <stdio.h>

#include <errno.h>
#include <unistd.h>
#include <sys/select.h>

#include "event.h"
#include "wm.h"
#include "keyboard.h"
#include "mouse.h"

void process_events()
{
	int keyb_fd = get_keyboard_fd();
	int mouse_fd = get_mouse_fd();

	for(;;) {
		wm->process_windows();

		fd_set read_set;

		FD_ZERO(&read_set);
		FD_SET(keyb_fd, &read_set);
		FD_SET(mouse_fd, &read_set);

		int maxfd = keyb_fd > mouse_fd ? keyb_fd : mouse_fd;

		while(select(maxfd + 1, &read_set, 0, 0, 0) == -1 && errno == EINTR);

		if(FD_ISSET(keyb_fd, &read_set)) {
			process_keyboard_event();
		}
		if(FD_ISSET(mouse_fd, &read_set)) {
			process_mouse_event();
		}
	}
}
#endif // WINNIE_FBDEV
