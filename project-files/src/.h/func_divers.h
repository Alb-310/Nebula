#ifndef FUNC_DIVERS_H
#define FUNC_DIVERS_H

void Resize(gdImagePtr im, FILE *out, char *path, int width, int height);

void Crop(gdImagePtr im, FILE *out, char *path,int x, int y, int width,
            int height);

void Rotate(gdImagePtr im, FILE *out, char *path,int angle);

void Add_text(gdImagePtr im, FILE *out, char *path, char *font, int x,
                int y, int width, int height, int color, double size,
                        double angle, char* text);

void picture_insertion(gdImagePtr src, FILE *out, gdImagePtr dst, char *path, int dst_x,int dst_y, float src_dim_percent);

void add_motif(gdImagePtr dst, FILE *out, char* path, char *motif,int dst_x,int dst_y,float src_dim_percent);

#endif

