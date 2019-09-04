#ifndef FONT_
#define FONT_
typedef struct _font {
    int height;
    int width;
    char *pixel;
} Font;

extern void putStringLarge(Font *fp, int x, int y, char *p, int color);
extern Font * read_font(char *pixel_file);
extern void destroy_font(Font *ip);
#endif

