#ifndef DRAWING_H
#define DRAWING_H

void set_pixel (gdImagePtr im, FILE *out, int x, int y, int r, int g, int b,
                    int a, int tks, int type, int zoom, char *path);

void line_to (gdImagePtr im, FILE *out, int x1, int y1, int x2, int y2, 
                int r, int g, int b, int a, int tks, int type, int zoom, char *path);

void fill (gdImagePtr im, FILE *out, int x, int y, int r, int g, int b,
                    int a, char *path);

#endif;

