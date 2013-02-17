#include <errno.h>
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

#define BN_LEFT		1
#define BN_RIGHT	2
#define BN_MIDDLE	4


static int dev_fd = -1;	// file descriptor for /dev/psaux
static Rect bounds;
static int pointer_x, pointer_y;
static int bnstate;

bool init_mouse()
{
	if((dev_fd = open("/dev/psaux", O_RDONLY)) == -1) {
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

	/* - process each event and update the pointer and button state
	 * - send each pointer move and button press/release to the tompost window
	 *   with the pointer on it.
	 */
}

void get_pointer_pos(int *x, int *y)
{
	*x = pointer_x;
	*y = pointer_y;
}

int get_button_state(int bn)
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


int read_mouse()
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
