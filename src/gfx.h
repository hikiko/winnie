#ifndef GFX_H_
#define GFX_H_

#include "geom.h"
#include "pixmap.h"

bool init_gfx();
void destroy_gfx();

unsigned char *get_framebuffer();
Pixmap *get_framebuffer_pixmap();

Rect get_screen_size();
int get_color_depth();

void clear_screen(int r, int g, int b);
void fill_rect(const Rect &rect, int r, int g, int b);

void set_cursor_visibility(bool visible);

void blit(unsigned char *src_img, const Rect &src_rect, unsigned char* dest_img,
		const Rect &dest_rect, int dest_x, int dest_y);

void blit_key(unsigned char *src_img, const Rect &src_rect, unsigned char* dest_img,
		const Rect &dest_rect, int dest_x, int dest_y, int key_r, int key_g, int key_b);

void gfx_update();

void wait_vsync(); // vertical synchronization

#endif //GFX_H_
