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

#ifndef MOUSE_H_
#define MOUSE_H_

struct Rect;

bool init_mouse();
void destroy_mouse();

void set_mouse_bounds(const Rect &rect);

int get_mouse_fd();
void process_mouse_event();

void get_pointer_pos(int *x, int *y);
int get_button_state();
int get_button(int bn);

#endif	// MOUSE_H_
