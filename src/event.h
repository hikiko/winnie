#ifndef EVENT_H_
#define EVENT_H_

class Window;

typedef void (*DisplayFuncType)(Window* win);
typedef void (*KeyboardFuncType)(Window* win, int key, bool pressed);
typedef void (*MouseButtonFuncType)(Window *win, int bn, bool pressed, int x, int y);
typedef void (*MouseMotionFuncType)(Window *win, int x, int y);

struct Callbacks {
	DisplayFuncType display;
	KeyboardFuncType keyboard;
	MouseButtonFuncType button;
	MouseMotionFuncType motion;
};

void process_events();

#endif
