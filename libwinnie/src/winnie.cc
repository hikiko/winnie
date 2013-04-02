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
#include <stdint.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include "keyboard.h"
#include "mouse.h"
#include "shalloc.h"
#include "winnie.h"

static Subsys *subsys;

bool winnie_init()
{
	if(!init_shared_memory()) {
		return false;
	}

	if(!(subsys = (Subsys*)sh_malloc(sizeof *subsys))) {
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
	destroy_window_manager();

	sh_free(subsys);

	destroy_shared_memory();
}

static int fd;
static void *pool;

bool winnie_open()
{
	if(((fd = shm_open(SHMNAME, O_RDWR, S_IRWXU)) == -1)) {
		fprintf(stderr, "Failed to open shared memory: %s\n", strerror(errno));
		return false;
	}

	if((pool = mmap(0, POOL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == (void*)-1) {
		fprintf(stderr, "Failed to map shared memory: %s\n", strerror(errno));
		return false;
	}

	subsys = (Subsys*)pool;

	if(!client_open_gfx(pool, subsys->graphics_offset)) {
		fprintf(stderr, "Failed to open graphics.\n");
		return false;
	}

	if(!client_open_keyboard(pool, subsys->keyboard_offset)) {
		fprintf(stderr, "Failed to open keyboard.\n");
		return false;
	}

	if(!client_open_mouse(pool, subsys->mouse_offset)) {
		fprintf(stderr, "Failed to open mouse.\n");
		return false;
	}

	if(!client_open_text(pool, subsys->text_offset)) {
		fprintf(stderr, "Failed to open text.\n");
		return false;
	}

	if(!client_open_wm(pool, subsys->wm_offset)) {
		fprintf(stderr, "Failed to open the window manager.\n");
		return false;
	}

	return true;
}

void winnie_close()
{
	client_close_gfx();
	client_close_keyboard();
	client_close_mouse();
	client_close_text();
	client_close_wm();

	if(munmap(pool, POOL_SIZE) == -1) {
		fprintf(stderr, "Failed to unmap shared memory: %s\n", strerror(errno));
	}
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

Subsys *get_subsys()
{
	return subsys;
}
