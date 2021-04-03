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

struct BrushType {
    int *pencil;
    // int *marker;
};

void set_brush_type(gdImagePtr im, struct BrushType **b, struct Color *c)
{
    int pe = 4;
    // int ma = 1;

    (*b)->pencil = malloc(pe * sizeof(int));
    // b->marker = malloc(ma * sizeof(int));

    int p = gdImageColorAllocateAlpha(im, c->r, c->g, c->b, c->a);
    int pc = gdImageColorAllocateAlpha(im, c->r, c->g, c->b, c->a + 15);
    int pf = gdImageColorAllocateAlpha(im, c->r, c->g, c->b, c->a - 30);

    (*b)->pencil[0] = p;
    (*b)->pencil[1] = pc;
    (*b)->pencil[2] = pf;
    (*b)->pencil[3] = pc;
}

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
    color_info->a = color_info->a + 127 + type_modif;
    int color = gdImageColorAllocateAlpha(im, color_info->r, color_info->g, color_info->b, color_info->a);
    
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

    struct BrushType *b = malloc(sizeof(struct BrushType));
    set_brush_type(im, &b, color_info);
    gdImageSetStyle(im, b->pencil, 4);

    while (point != NULL)
    {
        if(point->next == NULL)
            break;
        tmp = point;
        point = point->next;
        gdImageLine(im, tmp->x, tmp->y, point->x, point->y, gdStyled);
    }  
    
    gdImagePng(im, out);
    free(b->pencil);
    free(b);
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