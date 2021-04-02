#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include "gd.h"

struct Point {
    int x;
    int y;
    int red, green, blue, white;
    struct Point *next;
};

void set_pixel (gdImagePtr im, FILE *out, int x, int y, int r, int g, int b,
                    int a, int tks, int type, int zoom, char *path)
{
    out = fopen(path, "wb");
    int type_modif;
    if (type == 0) // Pinceau
        type_modif = 0;
    if (type == 1) // Crayon de papier
        type_modif = 50;
    if (type == 2) // Marqueur
        type_modif = 25;
    int color = gdImageColorAllocate(im, r, g, b);
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
                int r, int g, int b, int a, int tks, int type, int zoom, char *path)
{
    out = fopen(path, "wb");
    int type_modif;
    // tks *= zoom;
    int tks_modif;
    struct Point *tmp;
    struct Point *point = point_list;

    if (type == 0) // Pinceau
        type_modif = 0;
    if (type == 1) // Crayon de papier
        type_modif = 50;
    if (type == 2) // Marqueur
        type_modif = 25;

    int color = gdImageColorAllocate(im, r, g, b);
    if(color == 0){
        errx(EXIT_FAILURE, "Couldn't create color.");
    }

    if (tks == 1)
        tks_modif = 0;
    else if (tks == 3)
        tks_modif = 1;
    else if (tks == 5)
        tks_modif = 2;
    else
        tks_modif = 3;

    gdImageSetThickness(im, tks_modif);

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

void fill (gdImagePtr im, FILE *out, int x, int y, int r, int g, int b,
                    int a, char *path)
{
    out = fopen(path, "wb");
    int color = gdImageColor(im, r, g, b, a);
    gdImageFill(im, x, y, color);
    gdImagePng(im, out);
    fclose(out);
}