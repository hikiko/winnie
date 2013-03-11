#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winnie.h"

static void display(Window *win);
static void keyboard(Window *win, int key, bool pressed);
static void button(Window *win, int bn, bool pressed, int x, int y);
static void motion(Window *win, int x, int y);
static void cleanup();

int main()
{
	if(!winnie_init()) {
		exit(1);
	}

	atexit(cleanup);

	Window *win1 = new Window;
	win1->set_title("Clipping the win title");
	win1->move(200, 100);
	win1->resize(200, 300);
	win1->set_display_callback(display);
	win1->set_keyboard_callback(keyboard);
	win1->set_mouse_button_callback(button);
	win1->set_mouse_motion_callback(motion);

	Window *win2 = new Window;
	win2->set_title("window 2");
	win2->move(300, 100);
	win2->resize(200, 300);
	win2->set_display_callback(display);
	win2->set_keyboard_callback(keyboard);
	win2->set_mouse_button_callback(button);
	win2->set_mouse_motion_callback(motion);

	wm->add_window(win1);
	wm->add_window(win2);

	Pixmap bg;
	if(!(bg.load("data/bg.ppm"))) {
		fprintf(stderr, "failed to load pixmap\n");
	}

	wm->set_background(&bg);

	while(1) {
		process_events();
	}
}

static void display(Window *win)
{
	fill_rect(win->get_absolute_rect(), 128, 128, 128);
}

static void keyboard(Window *win, int key, bool pressed)
{
	switch(key) {
	case 'q':
		exit(0);
	}
}

static void button(Window *win, int bn, bool pressed, int x, int y)
{
	printf("WINDOW(%p) button %d %s\n", (void*)win, bn, pressed ? "press" : "release");
}

static void motion(Window *win, int x, int y)
{
	printf("WINDOW(%p) motion %d %d\n", (void*)win, x, y);
}

static void cleanup()
{
	winnie_shutdown();
}
