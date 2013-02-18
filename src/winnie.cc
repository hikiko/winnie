#include "winnie.h"
#include "keyboard.h"
#include "mouse.h"

bool winnie_init()
{
	if(!init_gfx()) {
		return false;
	}

	if(!init_keyboard()) {
		return false;
	}

	if(!init_mouse()) {
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
}
