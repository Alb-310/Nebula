#ifndef FUNC_DIVERS_H
#define FUNC_DIVERS_H

void Resize(char *filename, int width, int height);

void Crop(char *filename,int x,int y, int width,int height);

void Rotate(char *filename,int angle);

void Add_text(char *filename,char *font,int x,int y,int width,int height,int color,double size,double angle,char* text);

void picture_insertion(char *filename_destination,char *filename_source,int dst_x,int dst_y,float src_dim_percent);

void add_motif(char *filename_destination,char *motif,int dst_x,int dst_y, float src_dim_percent);

void collage(char*filename_background,char *filename_first_picture,char *filename_second_picture,char *filename_third_picture,char *filename_fourth_picture);

void collage_2(char*filename_background,char *filename_first_picture,char *filename_second_picture,char *filename_third_picture,char *filename_fourth_picture);

void collage_3(char*filename_background,char *filename_first_picture,char *filename_second_picture,char *filename_third_picture,char *filename_fourth_picture);

void collage_4(char *filename_background,char *filename_first_picture,char *filename_second_picture,char *filename_third_picture);

#endif

