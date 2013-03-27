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

#ifdef WINNIE_SDL
#include <SDL/SDL.h>

#include "mouse.h"
#include "shalloc.h"
#include "wm.h"
#include "window.h"
#include "winnie.h"

extern SDL_Event sdl_event;

struct Mouse {
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
	return true;
}

void destroy_mouse()
{
	sh_free(mouse);
}

void set_mouse_bounds(const Rect &rect)
{
}

int get_mouse_fd()
{
	return -1;
}

void process_mouse_event()
{
	int bn;
	MouseMotionFuncType motion_callback = 0;
	MouseButtonFuncType button_callback = 0;

	Window *win;
	if(!(win = wm->get_grab_window())) {
		win = wm->get_window_at_pos(mouse->pointer_x, mouse->pointer_y);
		if(win) {
			wm->set_focused_window(win);
		}
		else {
			wm->set_focused_window(0);
		}
	}

	switch(sdl_event.type) {
	case SDL_MOUSEMOTION:
		mouse->pointer_x = sdl_event.motion.x;
		mouse->pointer_y = sdl_event.motion.y;
		if(win && (motion_callback = win->get_mouse_motion_callback())) {
			Rect rect = win->get_absolute_rect();
			motion_callback(win, mouse->pointer_x - rect.x, mouse->pointer_y - rect.y);
		}
		break;

	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEBUTTONDOWN:
		bn = sdl_event.button.button - SDL_BUTTON_LEFT;
		if(sdl_event.button.state == SDL_PRESSED) {
			mouse->bnstate |= 1 << bn;
		}
		else {
			mouse->bnstate &= ~(1 << bn);
		}
		if(win && (button_callback = win->get_mouse_button_callback())) {
			Rect rect = win->get_absolute_rect();
			button_callback(win, bn, sdl_event.button.state, mouse->pointer_x - rect.x, mouse->pointer_y - rect.y);
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
#endif // WINNIE_SDL
