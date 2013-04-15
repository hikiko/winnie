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

#ifndef EVENT_H_
#define EVENT_H_

class Window;

typedef void (*DisplayFuncType)(Window* win);
typedef void (*KeyboardFuncType)(Window* win, int key, bool pressed);
typedef void (*MouseButtonFuncType)(Window *win, int bn, bool pressed, int x, int y);
typedef void (*MouseMotionFuncType)(Window *win, int x, int y);
typedef void (*TimerFuncType)(Window *win);

struct Callbacks {
	DisplayFuncType display;
	KeyboardFuncType keyboard;
	MouseButtonFuncType button;
	MouseMotionFuncType motion;
	TimerFuncType timer;
};

void process_events();

#endif
