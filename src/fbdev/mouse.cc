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
#include "window.h"
#include "wm.h"

#define BN_LEFT		1
#define BN_RIGHT	2
#define BN_MIDDLE	4

static int read_mouse();

static int dev_fd = -1;	// file descriptor for /dev/psaux
static Rect bounds;
static int pointer_x, pointer_y;
static int bnstate;

bool init_mouse()
{
	if((dev_fd = open("/dev/psaux", O_RDONLY | O_NONBLOCK)) == -1) {
		fprintf(stderr, "Cannot open /dev/psaux : %s\n", strerror(errno));
		return false;
	}

	set_mouse_bounds(get_screen_size());
	return true;
}

void destroy_mouse()
{
	if(dev_fd != -1) {
		close(dev_fd);
		dev_fd = -1;
	}
}

void set_mouse_bounds(const Rect &rect)
{
	bounds = rect;
}

int get_mouse_fd()
{
	return dev_fd;
}

void process_mouse_event()
{
	/* TODO:
	 * - read all pending events from mouse fd (use O_NONBLOCK so that
	 *   read will return -1 when there are no more events instead of blocking).
	 */

	int prev_state = bnstate;
	int prev_x = pointer_x;
	int prev_y = pointer_y;

	if(read_mouse() == -1) {
		return;
	}

	Window *top = wm->get_window_at_pos(pointer_x, pointer_y);
	if(top) {
		wm->set_focused_window(top);
	}
	else {
		wm->set_focused_window(0);
	}

	 /* - send each pointer move and button press/release to the topmost window
	 *   with the pointer on it.
	 */

	int dx = pointer_x - prev_x;
	int dy = pointer_y - prev_y;

	if((dx || dy) && top) {
		MouseMotionFuncType motion_callback = top->get_mouse_motion_callback();
		if(motion_callback) {
			Rect rect = top->get_absolute_rect();
			motion_callback(top, pointer_x - rect.x, pointer_y - rect.y);
		}
	}

	MouseButtonFuncType button_callback;
	if((bnstate != prev_state) && top && (button_callback = top->get_mouse_button_callback())) {
		int num_bits = sizeof bnstate * CHAR_BIT;
		for(int i=0; i<num_bits; i++) {
			int s = (bnstate >> i) & 1;
			int prev_s = (prev_state >> i) & 1;
			if(s != prev_s) {
				Rect rect = top->get_absolute_rect();
				button_callback(top, i, s, pointer_x - rect.x, pointer_y - rect.y);
			}
		}
	}
}

void get_pointer_pos(int *x, int *y)
{
	*x = pointer_x;
	*y = pointer_y;
}

int get_button_state()
{
	return bnstate;
}

int get_button(int bn)
{
	if(bn < 0 || bn >= 3) {
		return 0;
	}
	return (bnstate & (1 << bn)) != 0;
}

static int read_mouse()
{
	int rd;
	signed char state[3] = {0, 0, 0};

	if((rd = read(dev_fd, state, 3)) == -1) {
		fprintf(stderr, "Unable to get mouse state : %s\n", strerror(errno));
		return -1;
	}

	bnstate = state[0] & 7;
	pointer_x += state[1];
	pointer_y -= state[2];

	if(pointer_x < bounds.x) {
		pointer_x = bounds.x;
	}

	if(pointer_y < bounds.y) {
		pointer_y = bounds.y;
	}

	if(pointer_x > bounds.x + bounds.width - 1) {
		pointer_x = bounds.x + bounds.width - 1;
	}

	if(pointer_y > bounds.y + bounds.height - 1) {
		pointer_y = bounds.y + bounds.height - 1;
	}

	return 0;
}
#endif // WINNIE_FBDEV
