#ifndef DRAWING_H
#define DRAWING_H

int set_pixel (gdImagePtr im, FILE *out, int x, int y, void* c, int tks, int type, int zoom, char *path);

void line_to (gdImagePtr im, FILE *out, void *point_list, void* c, int tks, int type, int zoom, char *path);

void erase (gdImagePtr im, FILE *out, void *point_list, void* c, int tks, int zoom, char *path);

void wipe(gdImagePtr im, FILE *out, void *point_list, int *array, int *dw_array, 
                            int width, int zoom, char *path);
                        
void fill (gdImagePtr im, FILE *out, void* src, void* dst, int x, int y, char *path);

#endif

