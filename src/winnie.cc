#include "winnie.h"
#include "keyboard.h"

bool winnie_init()
{
	if(!init_gfx()) {
		return false;
	}

	if(!init_keyboard()) {
		return false;
	}

	wm->invalidate_region(get_screen_size());
	return true;
}

void winnie_shutdown()
{
	destroy_gfx();
	destroy_keyboard();
}
