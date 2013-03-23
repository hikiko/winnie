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
#include <stdlib.h>
#include <SDL/SDL.h>

#include "event.h"
#include "keyboard.h"
#include "mouse.h"
#include "wm.h"

SDL_Event sdl_event;
void process_events()
{
	wm->process_windows();
	if(!SDL_WaitEvent(&sdl_event)) {
		return;
	}

	switch(sdl_event.type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		process_keyboard_event();
		break;
	case SDL_MOUSEMOTION:
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		process_mouse_event();
		break;
	case SDL_QUIT:
		exit(0);
	default:
		break;
	}
}

#endif // WINNIE_SDL
