/*
winnie - an experimental window system

Copyright (C) 2013 Eleni Maria Stea

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Author: Eleni Maria Stea <elene.mst@gmail.com>
*/

#include <ft2build.h>
#include <freetype/freetype.h>

#include "gfx.h"
#include "shalloc.h"
#include "text.h"
#include "winnie.h"

#define DPI 72
#define FONT_PATH "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf"
#define FONT_SIZE 16

static int draw_glyph(Pixmap *pixmap, int x, int y, char c);

struct Text {
	FT_Library ft_lib;
	FT_Face ft_face;
	int text_x, text_y;
	int text_color[3];
};

static Text *text;

bool init_text()
{
	if(!(text = (Text*)sh_malloc(sizeof *text))) {
		return false;
	}

	get_subsys()->text_offset = (int)((char*)text - (char*)get_pool());

	if(FT_Init_FreeType(&text->ft_lib)) {
		fprintf(stderr, "Failed to initialize the FreeType library!\n");
		return false;
	}

	if(FT_New_Face(text->ft_lib, FONT_PATH, 0, &text->ft_face)) {
		fprintf(stderr, "Failed to load font: %s\n", FONT_PATH);
		return false;
	}

	if(FT_Set_Char_Size(text->ft_face, 0, FONT_SIZE * 64, DPI, DPI)) {
		fprintf(stderr, "Failed to set font size\n");
		return false;
	}

	set_text_color(255, 255, 255);

	return true;
}

void destroy_text()
{
	sh_free(text);
}

bool client_open_text(void *smem_start, int offset)
{
	text = (Text*)((unsigned char*)smem_start + offset);
	return true;
}

void client_close_text()
{
}

void draw_text(const char *txt, Pixmap *pixmap)
{
	if(!pixmap) {
		pixmap = get_framebuffer_pixmap();
	}

	while(*txt != 0) {
		text->text_x += draw_glyph(pixmap, text->text_x, text->text_y, *txt);
		txt++;
	}
}

void set_text_position(int x, int y)
{
	text->text_x = x;
	text->text_y = y;

}

void set_text_color(int r, int g, int b)
{
	text->text_color[0] = r;
	text->text_color[1] = g;
	text->text_color[2] = b;
}

static int draw_glyph(Pixmap *pixmap, int x, int y, char c)
{
	if(FT_Load_Char(text->ft_face, c, FT_LOAD_RENDER)) {
		return 0;
	}

	x += text->ft_face->glyph->bitmap_left;
	y -= text->ft_face->glyph->bitmap_top;

	FT_Bitmap *ft_bmp = &text->ft_face->glyph->bitmap;
	unsigned char *bmp_ptr = ft_bmp->buffer;
	unsigned char *pxm_ptr = pixmap->get_image() + (pixmap->get_width() * y + x) * 4;

	Rect clipping_rect = get_clipping_rect();

	for(int i=0; i<ft_bmp->rows; i++) {
		int dest_y = i + y;
		if(dest_y >= clipping_rect.y + clipping_rect.height) {
			break;
		}

		if(dest_y >= clipping_rect.y) {
			for(int j=0; j<ft_bmp->width; j++) {
				int dest_x = j + x;

				if(dest_x >= clipping_rect.x + clipping_rect.width) {
					break;
				}

				if(bmp_ptr[j] && dest_x >= clipping_rect.x) {
					int a = (int)bmp_ptr[j];
					pxm_ptr[4 * j] = (a * text->text_color[0] + pxm_ptr[4 * j] * (255 - a)) / 255;
					pxm_ptr[4 * j + 1] = (a * text->text_color[1] + pxm_ptr[4 * j + 1] * (255 - a)) / 255;
					pxm_ptr[4 * j + 2] = (a * text->text_color[2] + pxm_ptr[4 * j + 2] * (255 - a)) / 255;
				}
			}
		}

		pxm_ptr += 4 * pixmap->get_width();
		bmp_ptr += ft_bmp->pitch;
	}

	return text->ft_face->glyph->advance.x >> 6;
}
