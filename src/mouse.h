#ifndef MOUSE_H_
#define MOUSE_H_

struct Rect;

bool init_mouse();
void destroy_mouse();

void set_mouse_bounds(const Rect &rect);

int get_mouse_fd();
void process_mouse_event();

void get_pointer_pos(int *x, int *y);
int get_button_state(int bn);
int get_button(int bn);

int read_mouse();

#endif	// MOUSE_H_
