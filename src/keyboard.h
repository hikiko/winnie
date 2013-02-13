#ifndef KEYBOARD_H_
#define KEYBOARD_H_

bool init_keyboard();
void destroy_keyboard();

int get_keyboard_fd();
void process_keyboard_event();

#endif	// KEYBOARD_H_
