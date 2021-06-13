#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <unistd.h>
#include "gd.h"
#include ".h/shared_queue.h"

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
                int type, char *path)
{
    out = fopen(path, "wb");
    struct Color *color_info = c;
    int type_modif;
    int tks_modif = 0;
    
    switch(type){
        case 2: type_modif = 90; break;
        case 3: type_modif = 60; tks_modif = 5; break;
        case 4: type_modif = 110; tks *= 7; break;
        default: type_modif = 0; break;
    }
    int alpha_old = color_info->a;
    color_info->a = color_info->a + 127 + type_modif;
    int color = gdImageColorAllocateAlpha(im, color_info->r,
                    color_info->g, color_info->b, color_info->a);
    
    

    gdImageSetThickness(im, tks + tks_modif);

    if (type != 4)
    {
        tks_modif /= 2;
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
    }

    else
        gdImageFilledEllipse(im, x, y, tks, tks, color);
    
    gdImagePng(im, out);
    color_info->a = alpha_old;
    fclose(out);
    
    return 0;
}

void line_to (gdImagePtr im, FILE *out, void *point_list, 
                void* c, int tks, int type, char *path)
{
    
    out = fopen(path, "wb");
    struct Color *color_info = c;    
    int type_modif;
    // tks *= zoom;
    int tks_modif = 0;
    struct Point *tmp;
    struct Point *point = point_list;

    switch(type){
        case 2: type_modif = 90; break;
        case 3: type_modif = 60; tks_modif = 5; break;
        case 4: type_modif = 110; tks *= 7; break;
        default: type_modif = 0; break;
    }
    int alpha_old = color_info->a;
    color_info->a = color_info->a + 127 + type_modif;
    int color = 
        gdImageColorAllocateAlpha(im, color_info->r, color_info->g,
                                    color_info->b, color_info->a);

    

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
        if (type != 4)
            gdImageLine(im, tmp->x, tmp->y, point->x, point->y, color);
        else
            gdImageFilledEllipse(im, tmp->x, tmp->y, tks, tks, color);
    }  
    
    gdImagePng(im, out);
    color_info->a = alpha_old;
    //free(b->pencil);
    //free(b);
    fclose(out);
}

void erase(gdImagePtr im, FILE *out, void *point_list, 
                void* c, int tks, char *path)
{
    
    out = fopen(path, "wb");
    struct Color *color_info = c;    
    // tks *= zoom;
    //int tks_modif = 0;
    struct Point *tmp;
    struct Point *point = point_list;
    int color = 
        gdImageColorAllocate(im, color_info->r, color_info->g,
                                    color_info->b);

    

    /*struct BrushType *b = malloc(sizeof(struct BrushType));
    set_brush_type(im, &b, color_info);
    gdImageSetStyle(im, b->pencil, 4);*/

    while (point != NULL)
    {
        if(point->next == NULL)
            break;
        tmp = point;
        point = point->next;
        gdImageFilledEllipse(im, tmp->x, tmp->y, tks * 7, tks * 7, color);
    }  
    
    gdImagePng(im, out);
    //free(b->pencil);
    //free(b);
    fclose(out);
}

void wipe(gdImagePtr im, FILE *out, void *point_list, int *array, int *dw_array, 
                            int width, char *path)
{
    
    out = fopen(path, "wb");    
    // tks *= zoom;
    struct Point *tmp;
    struct Point *point = point_list;
    int *cp_array = array;
    
    /*struct BrushType *b = malloc(sizeof(struct BrushType));
    set_brush_type(im, &b, color_info);
    gdImageSetStyle(im, b->pencil, 4);*/

    while (point != NULL)
    {
        if(point->next == NULL)
            break;
        tmp = point;
        point = point->next;
        if (dw_array[tmp->x * width + tmp->y])
            gdImageSetPixel(im, tmp->x, tmp->y, cp_array[tmp->x * width + tmp->y]);
    }  
    
    gdImagePng(im, out);
    //free(b->pencil);
    //free(b);
    fclose(out);
}

void fill (gdImagePtr im, FILE *out, void *src, void *dst, int x, int y,
                char *path)
{
    out = fopen(path, "wb");
    struct Color *src_info = src;
    struct Color *dst_info = dst;
    
    int color_dst = gdImageColorAllocate(im, dst_info->r, dst_info->g,
                                                        dst_info->b);
    
    int width = gdImageSX(im);
    int height = gdImageSY(im);    
    int born = 48; 
    int *m = calloc(height*width, sizeof(int));

    shared_queue *sq = shared_queue_new();

    shared_queue_push(sq, x, y);

    while (sq->size > 0){
        int x, y;
        shared_queue_pop(sq, &x, &y);
        if (x < 0 || x > width || y > height || y < 0 ||
            m[y * width + x])
            continue;

        int pixel = gdImageGetTrueColorPixel(im, x, y);
        int r = gdImageRed(im, pixel);
        int b = gdImageBlue(im, pixel);
        int g = gdImageGreen(im, pixel);

        int is_set = r >= src_info->r - born && r <= src_info->r + born 
                && g >= src_info->g - born && g <= src_info->g + born
                && b >= src_info->b - born && b <= src_info->b + born;
        
        m[y * width + x] = 1;

        if(is_set)
            gdImageSetPixel(im, x, y, color_dst);
        else
            continue;  

        shared_queue_push(sq, x+1, y);
        shared_queue_push(sq, x-1, y);
        shared_queue_push(sq, x, y+1); 
        shared_queue_push(sq, x, y-1); 
    }

    //gdImageFill(im, x, y, color_dst);
    //free(tab);
    gdImagePng(im, out);
    fclose(out);
}
/*
void main ()
{
    gdImagePtr im = gdImageCreateFromFile("../../test-files/buzz.jpg");
    FILE *out;
    struct Color *src_info = malloc(sizeof(struct Color));
    struct Color *dst_info = malloc(sizeof(struct Color));
    src_info->r = 50;
    src_info->g = 50;
    src_info->b = 200;
    src_info->a = 255;
    dst_info->r = 255;
    dst_info->g = 255;
    dst_info->b = 0;
    dst_info->a = 255;
    fill(im, out, 340, 210, (void*)src_info, (void*)dst_info, "final.png");
    free(src_info);
    free(dst_info);
}
*/
