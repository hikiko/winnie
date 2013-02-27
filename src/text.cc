#include <ft2build.h>
#include <freetype/freetype.h>

#include "gfx.h"
#include "text.h"

#define DPI 72
#define FONT_PATH "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf"
#define FONT_SIZE 16

static int draw_glyph(Pixmap *pixmap, int x, int y, char c);

static FT_Library ft_lib;
static FT_Face ft_face;

static int text_x, text_y;
static int text_color[3] = {255, 255, 255};

bool init_text()
{
	if(FT_Init_FreeType(&ft_lib)) {
		fprintf(stderr, "Failed to initialize the FreeType library!\n");
		return false;
	}

	if(FT_New_Face(ft_lib, FONT_PATH, 0, &ft_face)) {
		fprintf(stderr, "Failed to load font: %s\n", FONT_PATH);
		return false;
	}

	if(FT_Set_Char_Size(ft_face, 0, FONT_SIZE * 64, DPI, DPI)) {
		fprintf(stderr, "Failed to set font size\n");
		return false;
	}

	return true;
}

void draw_text(const char *txt, Pixmap *pixmap)
{
	if(!pixmap) {
		pixmap = get_framebuffer_pixmap();
	}

	while(*txt != 0) {
		text_x += draw_glyph(pixmap, text_x, text_y, *txt);
		txt++;
	}
}

void set_text_position(int x, int y)
{
	text_x = x;
	text_y = y;

}

void set_text_color(int r, int g, int b)
{
	text_color[0] = r;
	text_color[1] = g;
	text_color[2] = b;
}

static int draw_glyph(Pixmap *pixmap, int x, int y, char c)
{
	if(FT_Load_Char(ft_face, c, FT_LOAD_RENDER)) {
		return 0;
	}

	x += ft_face->glyph->bitmap_left;
	y -= ft_face->glyph->bitmap_top;

	FT_Bitmap *ft_bmp = &ft_face->glyph->bitmap;
	unsigned char *bmp_ptr = ft_bmp->buffer;
	unsigned char *pxm_ptr = pixmap->get_image() + (pixmap->get_width() * y + x) * 4;

	for(int i=0; i<ft_bmp->rows; i++) {
		for(int j=0; j<ft_bmp->width; j++) {
			if(bmp_ptr[j]) {
				int a = (int)bmp_ptr[j];
				pxm_ptr[4 * j] = (a * text_color[0] + pxm_ptr[4 * j] * (255 - a)) / 255;
				pxm_ptr[4 * j + 1] = (a * text_color[1] + pxm_ptr[4 * j + 1] * (255 - a)) / 255;
				pxm_ptr[4 * j + 2] = (a * text_color[2] + pxm_ptr[4 * j + 2] * (255 - a)) / 255;
			}
		}
		pxm_ptr += 4 * pixmap->get_width();
		bmp_ptr += ft_bmp->pitch;
	}

	return ft_face->glyph->advance.x >> 6;
}
