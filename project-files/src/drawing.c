#include <stdlib.h>
#include <stdio.h>
#include "gd.h"

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
    int color = gdImageColor(im, r, g, b, a - type_modif);
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
