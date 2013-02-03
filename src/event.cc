#include "event.h"

static DisplayFuncType display_func;
static KeyboardFuncType keyboard_func;
static MouseButtonFuncType mouse_button_func;
static MouseMotionFuncType mouse_motion_func;

void set_display_callback(DisplayFuncType display)
{
	display_func = display;
}

void set_keyboard_callback(KeyboardFuncType keyboard)
{
	keyboard_func = keyboard;
}

void set_mouse_button_callback(MouseButtonFuncType mouse_button)
{
	mouse_button_func = mouse_button;
}

void set_mouse_motion_callback(MouseMotionFuncType mouse_motion)
{
	mouse_motion_func = mouse_motion;
}

DisplayFuncType get_display_callback()
{
	return display_func;
}

KeyboardFuncType get_keyboard_callback()
{
	return keyboard_func;
}

MouseButtonFuncType get_mouse_button_callback()
{
	return mouse_button_func;
}

MouseMotionFuncType get_mouse_motion_callback()
{
	return mouse_motion_func;
}
