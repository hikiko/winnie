#ifndef GFX_H_
#define GFX_H_

struct Rect {
	int x;
	int y;
	int width;
	int height;
};

bool init_gfx();
void destroy_gfx();

unsigned char* get_framebuffer();
Rect get_screen_size();
int get_color_depth();

void clear_screen(int r, int g, int b);

void set_cursor_visibility(bool visible);

#endif //GFX_H_
