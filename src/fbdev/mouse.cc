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
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "geom.h"
#include "gfx.h"
#include "mouse.h"
#include "shalloc.h"
#include "window.h"
#include "winnie.h"
#include "wm.h"

#define BN_LEFT		1
#define BN_RIGHT	2
#define BN_MIDDLE	4

static int read_mouse();

struct Mouse {
	int dev_fd;
	Rect bounds;
	int pointer_x;
	int pointer_y;
	int bnstate;
};

static Mouse *mouse;

bool init_mouse()
{
	if(!(mouse = (Mouse*)sh_malloc(sizeof *mouse))) {
		return false;
	}
	get_subsys()->mouse_offset = (int)((char*)mouse - (char*)get_pool());
	memset(mouse, 0, sizeof *mouse);

	mouse->dev_fd = -1;

	if((mouse->dev_fd = open("/dev/psaux", O_RDONLY | O_NONBLOCK)) == -1) {
		fprintf(stderr, "Cannot open /dev/psaux : %s\n", strerror(errno));
		return false;
	}

	set_mouse_bounds(get_screen_size());
	return true;
}

void destroy_mouse()
{
	if(mouse->dev_fd != -1) {
		close(mouse->dev_fd);
		mouse->dev_fd = -1;
	}
	sh_free(mouse);
}

void set_mouse_bounds(const Rect &rect)
{
	mouse->bounds = rect;
}

int get_mouse_fd()
{
	return mouse->dev_fd;
}

void process_mouse_event()
{
	/* TODO:
	 * - read all pending events from mouse fd (use O_NONBLOCK so that
	 *   read will return -1 when there are no more events instead of blocking).
	 */

	int prev_state = mouse->bnstate;
	int prev_x = mouse->pointer_x;
	int prev_y = mouse->pointer_y;

	if(read_mouse() == -1) {
		return;
	}

	Window *top;
	if(!(top = wm->get_grab_window())) {
		top = wm->get_window_at_pos(mouse->pointer_x, mouse->pointer_y);
		if(top) {
			wm->set_focused_window(top);
		}
		else {
			wm->set_focused_window(0);
		}
	}

	 /* - send each pointer move and button press/release to the topmost window
	 *   with the pointer on it.
	 */

	int dx = mouse->pointer_x - prev_x;
	int dy = mouse->pointer_y - prev_y;

	if((dx || dy) && top) {
		MouseMotionFuncType motion_callback = top->get_mouse_motion_callback();
		if(motion_callback) {
			Rect rect = top->get_absolute_rect();
			motion_callback(top, mouse->pointer_x - rect.x, mouse->pointer_y - rect.y);
		}
	}

	MouseButtonFuncType button_callback;
	if((mouse->bnstate != prev_state) && top && (button_callback = top->get_mouse_button_callback())) {
		int num_bits = sizeof mouse->bnstate * CHAR_BIT;
		for(int i=0; i<num_bits; i++) {
			int s = (mouse->bnstate >> i) & 1;
			int prev_s = (prev_state >> i) & 1;
			if(s != prev_s) {
				Rect rect = top->get_absolute_rect();
				button_callback(top, i, s, mouse->pointer_x - rect.x, mouse->pointer_y - rect.y);
			}
		}
	}
}

void get_pointer_pos(int *x, int *y)
{
	*x = mouse->pointer_x;
	*y = mouse->pointer_y;
}

int get_button_state()
{
	return mouse->bnstate;
}

int get_button(int bn)
{
	if(bn < 0 || bn >= 3) {
		return 0;
	}
	return (mouse->bnstate & (1 << bn)) != 0;
}

static int read_mouse()
{
	int rd;
	signed char state[3] = {0, 0, 0};

	if((rd = read(mouse->dev_fd, state, 3)) == -1) {
		fprintf(stderr, "Unable to get mouse state : %s\n", strerror(errno));
		return -1;
	}

	mouse->bnstate = state[0] & 7;
	mouse->pointer_x += state[1];
	mouse->pointer_y -= state[2];

	if(mouse->pointer_x < mouse->bounds.x) {
		mouse->pointer_x = mouse->bounds.x;
	}

	if(mouse->pointer_y < mouse->bounds.y) {
		mouse->pointer_y = mouse->bounds.y;
	}

	if(mouse->pointer_x > mouse->bounds.x + mouse->bounds.width - 1) {
		mouse->pointer_x = mouse->bounds.x + mouse->bounds.width - 1;
	}

	if(mouse->pointer_y > mouse->bounds.y + mouse->bounds.height - 1) {
		mouse->pointer_y = mouse->bounds.y + mouse->bounds.height - 1;
	}

	return 0;
}
#endif // WINNIE_FBDEV
