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

int set_pixel(gdImagePtr im, FILE *out, int x, int y, void* c, int tks, 
                int type, int zoom, char *path)
{
    printf("%ld\n", sizeof(zoom));
    out = fopen(path, "wb");
    struct Color *color_info = c;
    int type_modif;
    int tks_modif = 0;
    
    switch(type){
        case 2: type_modif = 90; break;
        case 3: type_modif = 60; tks_modif = 5; break;
        default: type_modif = 0; break;
    }
    int alpha_old = color_info->a;
    color_info->a = color_info->a + 127 + type_modif;
    int color = gdImageColorAllocateAlpha(im, color_info->r,
                    color_info->g, color_info->b, color_info->a);
    
    if(color == 0)
        errx(EXIT_FAILURE, "Couldn't create color.");

    gdImageSetThickness(im, tks + tks_modif);

    printf("tks: %d\n", tks);

    switch(tks){
        case 1: gdImageFilledRectangle(im, x - tks_modif, y - tks_modif,
                                x + tks_modif, y + tks_modif, color); break;
        case 3: gdImageFilledRectangle(im, x - 1 - tks_modif,
                                y - 1 - tks_modif, x + 1 + tks_modif,
                                y + 1 + tks_modif, color); break;
        case 5: gdImageFilledRectangle(im, x - 2 - tks_modif,
                                y - 2 - tks_modif, x + 2 + tks_modif,
                                y + 2 + tks_modif, color); break;
        case 7: gdImageFilledRectangle(im, x - 3 - tks_modif,
                                y - 3 - tks_modif, x + 3 + tks_modif,
                                y + 3 + tks_modif, color); break;
        default: errx(EXIT_FAILURE, "thickness is out of range.");
    }  
    
    gdImagePng(im, out);
    color_info->a = alpha_old;
    fclose(out);
    
    return 0;
}

void line_to (gdImagePtr im, FILE *out, void *point_list, 
                void* c, int tks, int type, int zoom, char *path)
{
    printf("%ld\n", sizeof(zoom));
    out = fopen(path, "wb");
    struct Color *color_info = c;    
    int type_modif;
    // tks *= zoom;
    int tks_modif = 0;
    struct Point *tmp;
    struct Point *point = point_list;

    switch(type){
        case 2: type_modif = 90; break;
        case 3: type_modif = 60; tks_modif = 10; break;
        default: type_modif = 0; break;
    }
    int alpha_old = color_info->a;
    color_info->a = color_info->a + 127 + type_modif;
    int color = 
        gdImageColorAllocateAlpha(im, color_info->r, color_info->g,
                                    color_info->b, color_info->a);

    if(color == 0)
        errx(EXIT_FAILURE, "Couldn't create color.");

    gdImageSetThickness(im, tks + tks_modif);

    /*struct BrushType *b = malloc(sizeof(struct BrushType));
    set_brush_type(im, &b, color_info);
    gdImageSetStyle(im, b->pencil, 4);*/

    while (point != NULL)
    {
        if(point->next == NULL)
            break;
        tmp = point;
        point = point->next;
        gdImageLine(im, tmp->x, tmp->y, point->x, point->y, color);
    }  
    
    gdImagePng(im, out);
    color_info->a = alpha_old;
    //free(b->pencil);
    //free(b);
    fclose(out);
}

void fill (gdImagePtr im, FILE *out, int x, int y, void* src, void* dst,
                char *path)
{
    out = fopen(path, "wb");
    struct Color *src_info = src;
    struct Color *dst_info = dst;
    int alpha_old = dst_info->a;
    dst_info->a = dst_info->a + 127;
    int color_src = gdImageColorAllocateAlpha(im, src_info->r, src_info->g,
                                                src_info->b, src_info->a);
    int color_dst = gdImageColorAllocateAlpha(im, dst_info->r, dst_info->g,
                                                dst_info->b, dst_info->a);
    if(color_src == 0 || color_dst == 0)
        errx(EXIT_FAILURE, "Couldn't create color.");

    int width = gdImageSX(im);
    int height = gdImageSY(im);

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            int truepixel = gdImageGetTrueColorPixel(im,x,y);
            int r = gdImageRed(im,truepixel);
            int b = gdImageBlue(im,truepixel);
            int g = gdImageGreen(im,truepixel);
            int born = 30;
            if(r >= src_info->r - born && r <= src_info->r + born 
                && g >= src_info->g - born && g <= src_info->g + born
                && b >= src_info->b - born && b <= src_info->b + born)
            {
                gdImageSetPixel(im, i, j, color_dst);
            }
        }
    }
    gdImagePng(im, out);
    dst_info->a = alpha_old;
    fclose(out);
}
