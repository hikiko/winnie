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
