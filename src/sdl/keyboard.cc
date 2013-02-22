#ifdef WINNIE_SDL
#include <SDL/SDL.h>

#include "keyboard.h"
#include "window.h"
#include "wm.h"

extern SDL_Event sdl_event;

bool init_keyboard()
{
	return true;
}

void destroy_keyboard()
{
}

int get_keyboard_fd()
{
	return -1;
}

void process_keyboard_event()
{
	int key = sdl_event.key.keysym.sym;

	Window *focused_win = wm->get_focused_window();
	if(focused_win) {
		KeyboardFuncType keyb_callback = focused_win->get_keyboard_callback();
		if(keyb_callback) {
			bool pressed = sdl_event.key.state == SDL_PRESSED;
			keyb_callback(focused_win, key, pressed);
		}
	}
}
#endif // WINNIE_SDL
