#ifdef WINNIE_SDL
#include <stdlib.h>
#include <SDL/SDL.h>

#include "event.h"
#include "keyboard.h"
#include "mouse.h"
#include "wm.h"

SDL_Event sdl_event;
void process_events()
{
	wm->process_windows();
	if(!SDL_WaitEvent(&sdl_event)) {
		return;
	}

	switch(sdl_event.type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		process_keyboard_event();
		break;
	case SDL_MOUSEMOTION:
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		process_mouse_event();
		break;
	case SDL_QUIT:
		exit(0);
	}
}

#endif // WINNIE_SDL
