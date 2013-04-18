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

enum {
	TIMER_EVENT = SDL_USEREVENT
};

SDL_Event sdl_event;
void process_events()
{
	wm->process_windows();
	if(!SDL_WaitEvent(&sdl_event)) {
		return;
	}

	do {
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

		case TIMER_EVENT:
			{
				Window *win = (Window*)sdl_event.user.data1;
				TimerFuncType func = win->get_timer_callback();
				if(func) {
					func(win);
				} else {
					fprintf(stderr, "timer gone off but window has no timer callback!\n");
				}
			}
			break;

		default:
			break;
		}
	} while(SDL_PollEvent(&sdl_event));
}

struct TimerData {
	SDL_TimerID sdl_timer;
	Window *win;
	TimerMode mode;
};

static unsigned int timer_callback(unsigned int interval, void *cls)
{
	TimerData *td = (TimerData*)cls;

	SDL_Event ev;
	ev.type = TIMER_EVENT;
	ev.user.data1 = td->win;
	SDL_PushEvent(&ev);

	if(td->mode == TIMER_ONESHOT) {
		delete td;
		return 0;
	}
	return interval;	// repeat at same interval
}

void set_window_timer(Window *win, unsigned int msec, TimerMode mode)
{
	if(!win->get_timer_callback()) {
		fprintf(stderr, "trying to start a timer without having a timer callback!\n");
		return;
	}
	TimerData *td = new TimerData;
	td->win = win;
	td->mode = mode;
	td->sdl_timer = SDL_AddTimer(msec, timer_callback, td);
}

#endif // WINNIE_SDL
