#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include "gd.h"

struct Point {
    int x;
    int y;
    struct Point *next;
};

struct Color {
    int r;
    int g;
    int b;
    int a;
};

void set_pixel(gdImagePtr im, FILE *out, int x, int y, void* c,int tks, int type, int zoom, char *path)
{
    out = fopen(path, "wb");
    struct Color *color_info = c;
    int type_modif;
    
    switch(type){
        case 2: type_modif = 75; break;
        case 3: type_modif = 115; break;
        default: type_modif = 0; break;
    }
    int color = gdImageColorAllocateAlpha(im, color_info->r, color_info->g, color_info->b, color_info->a + 127 + type_modif);
    
    // tks *= zoom;
    int tks_modif;
    if (tks == 1)
        gdImageSetPixel(im, x, y, color);
    else if (tks == 3)
        tks_modif = 1;
    else if (tks == 5)
        tks_modif = 2;
    else
        tks_modif = 3;
    gdImageFilledRectangle(im, x - tks_modif, y - tks_modif,
                                x + tks_modif, y + tks_modif, color);
    gdImagePng(im, out);
    fclose(out);
}

void line_to (gdImagePtr im, FILE *out, void *point_list, 
                void* c, int tks, int type, int zoom, char *path)
{
    out = fopen(path, "wb");
    struct Color *color_info = c;    
    int type_modif;
    // tks *= zoom;
    int tks_modif;
    struct Point *tmp;
    struct Point *point = point_list;

    switch(type){
        case 2: type_modif = 75; break;
        case 3: type_modif = 115; break;
        default: type_modif = 0; break;
    }
    int color = gdImageColorAllocateAlpha(im, color_info->r, color_info->g, color_info->b, color_info->a + 127 + type_modif);

    if(color == 0){
        errx(EXIT_FAILURE, "Couldn't create color.");
    }

    gdImageSetThickness(im, tks);

    while (point != NULL)
    {
        if(point->next == NULL)
            break;
        tmp = point;
        point = point->next;
        gdImageLine(im, tmp->x, tmp->y, point->x, point->y, color);
    }  
    
    gdImagePng(im, out);
    fclose(out);
}

void fill (gdImagePtr im, FILE *out, int x, int y, void* c, char *path)
{
    out = fopen(path, "wb");
    struct Color *color_info = c;
    int color = gdImageColorAllocateAlpha(im, color_info->r, color_info->g, color_info->b, color_info->a);
    gdImageFill(im, x, y, color);
    gdImagePng(im, out);
    fclose(out);
}