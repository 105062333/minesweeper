#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>

#include "console_draw3.h"
#include "kb_input2.h"


#include "image.h"


static char getpixel(Image *ip, int x, int y);
int getcolor(Image *ip, int x, int y);


Image * read_image(char *pixel_file, char *color_file)
{
    FILE *fid1, *fid2;
    int row, col, i, j;
    Image *p;

    fid1 = fopen(pixel_file, "r");
    fid2 = fopen(color_file, "r");
    if (fid1==NULL || fid2==NULL) {
        return NULL;
    } else {
        fscanf(fid1, "%d%d", &row, &col);
        fscanf(fid2, "%d%d", &row, &col); // supposedly, size should be the same
        p = (Image*) malloc(sizeof(Image));
        p->row = row;
        p->col = col;
        p->pixel = (char*) malloc(sizeof(char)*row*col);
        p->color = (int*) malloc(sizeof(int)*row*col);
        fscanf(fid1, "%*c");
        for (i=0; i<row; i++) {
            for (j=0; j<col; j++) {
                fscanf(fid1, "%c", & (p->pixel[i*col+j]));
                fscanf(fid2, "%1x", & (p->color[i*col+j]));
            }
            fscanf(fid1, "%*c");
            fscanf(fid2, "%*c");
        }
        fclose(fid1);
        fclose(fid2);
        return p;
    }
}

char getpixel(Image *ip, int x, int y)
{
    return ip->pixel[y*ip->col + x];
}

int getcolor(Image *ip, int x, int y)
{
    return ip->color[y*ip->col + x];
}


void show_image(Image *ip, int offx, int offy)
{
    int x, y;
    char c;
    for (y=0; y<ip->row; y++) {
            for (x=0; x<ip->col; x++) {
            c = getpixel(ip, x, y);
            if (c!=' ')
            drawChar(c,
                     x + offx,
                     y + offy, 0, getcolor(ip, x, y));
            }
    }
}

void destroy_image(Image *ip)
{
    free(ip->pixel);
    free(ip->color);
    free(ip);
}
