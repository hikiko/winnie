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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winnie.h"
#include "client_plugins.h"

static void cleanup();

int main()
{
	if(!winnie_init()) {
		return 1;
	}
	if(!init_client_plugins()) {
		return 1;
	}

	atexit(cleanup);

	Pixmap bg;
	if(bg.load("data/bg.ppm")) {
		wm->set_background(&bg);
	} else {
		wm->set_background_color(64, 64, 64);
	}


	while(1) {
		process_events();
	}
}

static void cleanup()
{
	destroy_client_plugins();
	winnie_shutdown();
}
