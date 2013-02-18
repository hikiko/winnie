#ifndef GFX_H_
#define GFX_H_

#include "geom.h"

bool init_gfx();
void destroy_gfx();

unsigned char *get_framebuffer();
Rect get_screen_size();
int get_color_depth();

void clear_screen(int r, int g, int b);
void fill_rect(const Rect &rect, int r, int g, int b);

void set_cursor_visibility(bool visible);

void blit(unsigned char *src_img, const Rect &src_rect, unsigned char* dest_img, int dest_x, int dest_y);

#endif //GFX_H_
