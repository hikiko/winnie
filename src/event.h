#ifndef EVENT_H_
#define EVENT_H_

typedef void (*DisplayFuncType)();
typedef void (*KeyboardFuncType)(int key, bool pressed);
typedef void (*MouseButtonFuncType)(int bn, bool pressed);
typedef void (*MouseMotionFuncType)(int x, int y);

void set_display_callback(DisplayFuncType display);
void set_keyboard_callback(KeyboardFuncType keyboard);
void set_mouse_button_callback(MouseButtonFuncType mouse_button);
void set_mouse_motion_callback(MouseMotionFuncType mouse_motion);

DisplayFuncType get_display_callback();
KeyboardFuncType get_keyboard_callback();
MouseButtonFuncType get_mouse_button_callback();
MouseMotionFuncType get_mouse_motion_callback();

#endif
