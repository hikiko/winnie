#include <stdio.h>

#include "gfx.h"

int main()
{
	if(!init_gfx()) {
		return 1;
	}

	set_cursor_visibility(false);

	unsigned char* fb = get_framebuffer();
	Rect scrn_sz = get_screen_size();

	for(int i=0; i<scrn_sz.height; i++) {
		for(int j=0; j<scrn_sz.width; j++) {
			unsigned char color0[3] = {255, 0, 0};
			unsigned char color1[3] = {0, 0, 255};
			unsigned char* color;

			//red blue chessboard 16 pxls size
			if(((j >> 4) & 1) == ((i >> 4) & 1)) {
				color = color0;
			}
			else {
				color = color1;
			}

			*fb++ = color[0];
			*fb++ = color[1];
			*fb++ = color[2];
			fb++;
		}
	}

	getchar();
	clear_screen(0, 0, 0);

	destroy_gfx();
}
