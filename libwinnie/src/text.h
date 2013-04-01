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

#ifndef TEXT_H_
#define TEXT_H_

bool init_text();
void destroy_text();

bool client_open_text(void *smem_start, int offset);
void client_close_text();

void draw_text(const char *txt, Pixmap *pixmap = 0);
void set_text_position(int x, int y);
void set_text_color(int r, int g, int b);

#endif // TEXT_H_
