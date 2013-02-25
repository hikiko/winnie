#ifdef WINNIE_SDL
#include <SDL/SDL.h>

#include "mouse.h"
#include "window.h"
#include "wm.h"

extern SDL_Event sdl_event;

static int pointer_x, pointer_y;
static int bnstate;

bool init_mouse()
{
	return true;
}

void destroy_mouse()
{
}

void set_mouse_bounds(const Rect &rect)
{
}

int get_mouse_fd()
{
	return -1;
}

void process_mouse_event()
{
	int bn;
	MouseMotionFuncType motion_callback = 0;
	MouseButtonFuncType button_callback = 0;

	Window *win;
	if(!(win = wm->get_grab_window())) {
		win = wm->get_window_at_pos(pointer_x, pointer_y);
		if(win) {
			wm->set_focused_window(win);
		}
		else {
			wm->set_focused_window(0);
		}
	}

	switch(sdl_event.type) {
	case SDL_MOUSEMOTION:
		pointer_x = sdl_event.motion.x;
		pointer_y = sdl_event.motion.y;
		if(win && (motion_callback = win->get_mouse_motion_callback())) {
			Rect rect = win->get_absolute_rect();
			motion_callback(win, pointer_x - rect.x, pointer_y - rect.y);
		}
		break;

	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEBUTTONDOWN:
		bn = sdl_event.button.button - SDL_BUTTON_LEFT;
		if(sdl_event.button.state == SDL_PRESSED) {
			bnstate |= 1 << bn;
		}
		else {
			bnstate &= ~(1 << bn);
		}
		if(win && (button_callback = win->get_mouse_button_callback())) {
			Rect rect = win->get_absolute_rect();
			button_callback(win, bn, sdl_event.button.state, pointer_x - rect.x, pointer_y - rect.y);
		}
	}
}

void get_pointer_pos(int *x, int *y)
{
	*x = pointer_x;
	*y = pointer_y;
}

int get_button_state()
{
	return bnstate;
}

int get_button(int bn)
{
	if(bn < 0 || bn >= 3) {
		return 0;
	}
	return (bnstate & (1 << bn)) != 0;
}
#endif // WINNIE_SDL
