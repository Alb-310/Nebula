#ifndef DRAWING_H
#define DRAWING_H

int set_pixel (gdImagePtr im, FILE *out, int x, int y, void* c, int tks, int type, int zoom, char *path);

void line_to (gdImagePtr im, FILE *out, void *point_list, void* c, int tks, int type, int zoom, char *path);

void fill (gdImagePtr im, FILE *out, int x, int y, void* src, void* dst, char *path);

#endif

