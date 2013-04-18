/*
includes code adapted from Torsmo:
Copyright (c) 2004, Hannu Saransaari and Lauri Hakkarainen
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <ORGANIZATION> nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>

#include "winnie.h"

#define START_WIDTH		200
#define START_HEIGHT	128

#define NUM_SAMPLES	64

enum {
	INFO_CPU,
	INFO_MEM,
	INFO_PROCS,
	INFO_RUN_PROCS
};

static struct information {
	unsigned int mask;
	float cpu_usage;
	unsigned int cpu_count;
	unsigned int procs, run_procs;
} info;

static double current_update_time, last_update_time;
static float cpugraph[NUM_SAMPLES];
static int cpugraph_top;

static Pixmap *bgimage;

static void gen_bgimage(int xsz, int ysz);

static void display(Window *win);
static void keyboard(Window *win, int key, bool pressed);
static void button(Window *win, int bn, bool pressed, int x, int y);
static void motion(Window *win, int x, int y);
static void timer(Window *win);

static void update_stat(void);
static double get_time(void);

extern "C" bool init()
{
	Window *win = new Window;
	win->set_title("cpu usage");
	win->move(600, 50);
	win->resize(START_WIDTH, START_HEIGHT);
	win->set_display_callback(display);
	win->set_keyboard_callback(keyboard);
	win->set_mouse_button_callback(button);
	win->set_mouse_motion_callback(motion);
	win->set_timer_callback(timer);

	wm->add_window(win);

	bgimage = new Pixmap;
	gen_bgimage(START_HEIGHT, START_HEIGHT);

	set_window_timer(win, 800, TIMER_REPEAT);
	return true;
}

extern "C" void cleanup()
{
	delete bgimage;
}

static void gen_bgimage(int xsz, int ysz)
{
	// allocate pixel buffer (or resize)
	bgimage->set_image(xsz, ysz);

	unsigned char *ptr = bgimage->pixels;
	for(int i=0; i<ysz; i++) {
		int green = 255 * i / (ysz - 1);
		int red = 255 - green;

		for(int j=0; j<xsz; j++) {
			*ptr++ = red;
			*ptr++ = green;
			*ptr++ = 0;
			*ptr++ = 255;
		}
	}
}

static void display(Window *win)
{
	Rect rect = win->get_absolute_rect();
	Pixmap *fb = get_framebuffer_pixmap();

	int cpu_usage = (int)(info.cpu_usage * 100.0f);

	char title[64];
	sprintf(title, "cpu usage: %d%%", cpu_usage);
	win->set_title(title);

	if(bgimage->width != rect.width || bgimage->height != rect.height) {
		gen_bgimage(rect.width, rect.height);
	}

	blit(bgimage->pixels, Rect(0, 0, bgimage->width, bgimage->height),
			fb->pixels, get_screen_size(), rect.x, rect.y);

	float pix_per_sample = (float)rect.width / (float)(NUM_SAMPLES - 1);
	int prev_x = 0;
	int prev_y = rect.height - cpugraph[cpugraph_top] * rect.height;

	for(int i=1; i<NUM_SAMPLES; i++) {
		int x = i * pix_per_sample;
		if(x >= rect.width || i == NUM_SAMPLES - 1) {
			x = rect.width - 1;
		}
		int y = rect.height - cpugraph[(cpugraph_top + i) % NUM_SAMPLES] * rect.height;

		/*int varr[] = {
			prev_x, rect.height,
			prev_x, prev_y,
			x, y,
			x, rect.height
		};*/
		int varr[] = {
			x, 0,
			x, y,
			prev_x, prev_y,
			prev_x, 0
		};

		for(int i=0; i<4; i++) {
			varr[i * 2] += rect.x;
			varr[i * 2 + 1] += rect.y;
		}

		draw_polygon(fb, varr, 0, 4, 0, 0, 0);

		prev_x = x;
		prev_y = y;
	}
}

static void keyboard(Window *win, int key, bool pressed)
{
	printf("WINDOW(%p) keyboard %d %s\n", (void*)win, key, pressed ? "press" : "release");
}

static void button(Window *win, int bn, bool pressed, int x, int y)
{
	printf("WINDOW(%p) button %d %s\n", (void*)win, bn, pressed ? "press" : "release");
}

static void motion(Window *win, int x, int y)
{
	printf("WINDOW(%p) motion %d %d\n", (void*)win, x, y);
}

static void timer(Window *win)
{
	current_update_time = get_time();
	update_stat();

	cpugraph[cpugraph_top] = info.cpu_usage;
	cpugraph_top = (cpugraph_top + 1) % NUM_SAMPLES;

	last_update_time = current_update_time;

	Rect dirty_rect = win->get_absolute_rect();
	dirty_rect.y -= 1;	// to touch the frame too
	wm->invalidate_region(dirty_rect);
}

static void update_stat(void)
{
	static unsigned int cpu_user, cpu_system, cpu_nice;
	static double last_cpu_sum;
	static int clock_ticks;

	static int rep;
	static FILE *stat_fp;
	char buf[256];

	if (stat_fp == NULL) {
		if(!(stat_fp = fopen("/proc/stat", "r"))) {
			if(!rep) {
				fprintf(stderr, "failed to open /proc/stat: %s\n", strerror(errno));
			}
			rep = 1;
		}
	} else {
		fseek(stat_fp, 0, SEEK_SET);
	}
	if (stat_fp == NULL) return;

	info.cpu_count = 0;

	while (!feof(stat_fp)) {
		if (fgets(buf, 255, stat_fp) == NULL)
			break;

		if (strncmp(buf, "procs_running ", 14) == 0) {
			sscanf(buf, "%*s %u", &info.run_procs);
			info.mask |= (1 << INFO_RUN_PROCS);
		}
		else if (strncmp(buf, "cpu ", 4) == 0) {
			sscanf(buf, "%*s %u %u %u", &cpu_user, &cpu_nice, &cpu_system);
			info.mask |= (1 << INFO_CPU);
		}
		else if (strncmp(buf, "cpu", 3) == 0 && isdigit(buf[3])) {
			info.cpu_count++;
		}
	}

	{
		double delta;
		delta = current_update_time - last_update_time;
		if (delta <= 0.001) return;

		if (clock_ticks == 0)
			clock_ticks = sysconf(_SC_CLK_TCK);

		info.cpu_usage = (cpu_user+cpu_nice+cpu_system - last_cpu_sum) / delta
			/ (double) clock_ticks / info.cpu_count;
		last_cpu_sum = cpu_user+cpu_nice+cpu_system;
	}
}

static double get_time(void)
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}
