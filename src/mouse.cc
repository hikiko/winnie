#include "mouse.h"
#include "geom.h"

static int dev_fd = -1;	// file descriptor for /dev/psaux
static Rect bounds;
static int pointer_x, pointer_y;

bool init_mouse()
{
	// TODO open /dev/psaux (see O_NONBLOCK comment below)
	return true;
}

void destroy_mouse()
{
	// TODO close /dev/psaux
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
	 * - process each event and update the pointer and button state
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
	// TODO
	return 0;
}
