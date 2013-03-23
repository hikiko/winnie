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

#include "keyboard.h"
#include "window.h"
#include "wm.h"

extern SDL_Event sdl_event;

bool init_keyboard()
{
	return true;
}

void destroy_keyboard()
{
}

int get_keyboard_fd()
{
	return -1;
}

void process_keyboard_event()
{
	int key = sdl_event.key.keysym.sym;

	Window *focused_win = wm->get_focused_window();
	if(focused_win) {
		KeyboardFuncType keyb_callback = focused_win->get_keyboard_callback();
		if(keyb_callback) {
			bool pressed = sdl_event.key.state == SDL_PRESSED;
			keyb_callback(focused_win, key, pressed);
		}
	}
}
#endif // WINNIE_SDL
