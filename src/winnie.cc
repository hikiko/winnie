#include <sys/time.h>

#include "keyboard.h"
#include "mouse.h"
#include "shalloc.h"
#include "winnie.h"

bool winnie_init()
{
	if(!init_shared_memory()) {
		return false;
	}

	if(!init_gfx()) {
		return false;
	}

	if(!init_window_manager()) {
		return false;
	}

	if(!init_keyboard()) {
		return false;
	}

	if(!init_mouse()) {
		return false;
	}

	if(!init_text()) {
		return false;
	}

	wm->invalidate_region(get_screen_size());
	return true;
}

void winnie_shutdown()
{
	destroy_gfx();
	destroy_keyboard();
	destroy_mouse();
	destroy_text();

	destroy_shared_memory();
}

long winnie_get_time()
{
	static struct timeval init_tv;
	struct timeval tv;

	gettimeofday(&tv, 0);

	if(!tv.tv_sec && !tv.tv_usec) {
		init_tv = tv;
		return 0;
	}

	return (tv.tv_usec - init_tv.tv_usec) / 1000 + (tv.tv_sec - init_tv.tv_sec) * 1000;
}
