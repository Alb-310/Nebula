#ifndef FUNC_DIVERS_H
#define FUNC_DIVERS_H

void Resize(char *filename, int width, int height);

void Crop(char *filename,int x,int y, int width,int height);

void Rotate(char *filename,int angle);

void Add_text(char *filename,char *font,int x,int y,int width,int height,int color,double size,double angle,char* text);

void picture_insertion(gdImagePtr src, FILE *out, gdImagePtr dst, char *path, int dst_x,int dst_y, float src_dim_percent);

void add_motif(char *filename_destination,char *motif,int dst_x,int dst_y, float src_dim_percent);

#endif

