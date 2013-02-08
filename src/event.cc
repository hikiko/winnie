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

		wm->process_windows();
	}
}
