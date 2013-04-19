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

#ifdef WINNIE_FBDEV
#include <list>
#include <stdio.h>

#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

#include "event.h"
#include "wm.h"
#include "keyboard.h"
#include "mouse.h"

struct TimerEvent {
	long time;
	Window *win;
	TimerMode mode;
	long interval;

	bool operator <(const TimerEvent &rhs) const { return time < rhs.time; }
};

static std::list<TimerEvent> timers;

void process_events()
{
	int keyb_fd = get_keyboard_fd();
	int mouse_fd = get_mouse_fd();

	for(;;) {
		wm->process_windows();

		fd_set read_set;

		FD_ZERO(&read_set);
		FD_SET(keyb_fd, &read_set);
		FD_SET(mouse_fd, &read_set);

		int maxfd = keyb_fd > mouse_fd ? keyb_fd : mouse_fd;

		struct timeval *timeout = 0;
		struct timeval tv;
		if(!timers.empty()) {
			long now = winnie_get_time();
			long next_timer = timers.front().time - now;

			tv.tv_sec = next_timer / 1000;
			tv.tv_usec = next_timer % 1000;
			timeout = &tv;
		}

		int numfd;
		while((numfd = select(maxfd + 1, &read_set, 0, 0, timeout)) == -1) {
			if(errno != EINTR) {
				break;
			}
		}

		if(numfd > 0) {
			if(FD_ISSET(keyb_fd, &read_set)) {
				process_keyboard_event();
			}
			if(FD_ISSET(mouse_fd, &read_set)) {
				process_mouse_event();
			}
		}

		bool added_timer = false;
		long now = winnie_get_time();
		while(!timers.empty() && now >= timers.front().time) {
			TimerEvent ev = timers.front();
			timers.pop_front();

			if(ev.mode == TIMER_REPEAT) {
				ev.time = now + ev.interval;
				timers.push_back(ev);
				added_timer = true;
			}

			Window *win = ev.win;
			TimerFuncType func = win->get_timer_callback();
			if(func) {
				func(win);
			} else {
				fprintf(stderr, "timer gone off but window has no timer callback\n");
			}
		}

		if(added_timer) {
			timers.sort();
		}
	}
}

void set_window_timer(Window *win, unsigned int msec, TimerMode mode)
{
	if(!win) {
		fprintf(stderr, "set_window_timer null window\n");
		return;
	}
	if(!win->get_timer_callback()) {
		fprintf(stderr, "trying to start a timer without having a timer callback\n");
		return;
	}

	TimerEvent ev;
	ev.interval = (long)msec;
	ev.time = winnie_get_time() + ev.interval;
	ev.win = win;
	ev.mode = mode;
	timers.push_back(ev);
	timers.sort();
}

#endif // WINNIE_FBDEV
