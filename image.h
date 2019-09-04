#ifndef IMAGE_
#define IMAGE_
typedef struct _image {
    int row;
    int col;
    char *pixel;
    int *color;
} Image;;


extern Image * read_image(char *pixel_file, char *color_file);
extern void show_image(Image *ip, int offx, int offy);
extern void destroy_image(Image *ip);

#endif

