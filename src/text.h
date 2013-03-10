#ifndef TEXT_H_
#define TEXT_H_

bool init_text();
void destroy_text();

void draw_text(const char *txt, Pixmap *pixmap = 0);
void set_text_position(int x, int y);
void set_text_color(int r, int g, int b);

#endif // TEXT_H_
