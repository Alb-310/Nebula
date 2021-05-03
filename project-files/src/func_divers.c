#include "gd.h"
#include <stdio.h>
#include <err.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void Resize(gdImagePtr im, FILE *out, char *path, int width, int height)
{
	out = fopen(path,"wb");
	im=gdImageScale(im,width,height);
	gdImagePng(im,out);
	fclose(out);
}

void Crop(gdImagePtr im, FILE *out, char *path,int x, int y, int width,
            int height)
{
	out = fopen(path,"wb");
	gdRect *crop=malloc(sizeof(gdRect));

	crop->x=x;
	crop->y=y;
	crop->width=width;
	crop->height=height;	
	im=gdImageCrop(im,crop);
	gdImagePng(im,out);
	gdFree(crop);
	fclose(out);

}

void Rotate(gdImagePtr im, FILE *out, char *path,int angle)
{
	out = fopen(path,"wb");
	im=gdImageRotateInterpolated(im,angle,0);
	if(im==NULL)
		errx(1,"rotation failure");
	gdImagePng(im,out);
	fclose(out);
}

void Add_text(gdImagePtr im, FILE *out, char *path, char *font, int x,
                int y, int width, int height, int color, double size,
                        double angle, char* text)
{
	out = fopen(path,"wb");
	int *brect = malloc(8*sizeof(int));
	brect[0]=x;
	brect[1]=y;
	brect[2]=x+width;
	brect[3]=y;
	brect[4]=x+width;
	brect[5]=y+height;
	brect[6]=x;
	brect[7]=y+height;
	gdImageStringFT(im,brect,color,font,size,angle,x,y,text);
	gdImagePng(im,out);
	fclose(out);
}

void picture_insertion(char *filename_destination, char *filename_source,int dst_x,int dst_y,float src_dim_percent)
{
	gdImagePtr src;
	gdImagePtr dst;
	FILE *out;
	out = fopen("result.png","wb");
	src = gdImageCreateFromFile(filename_source);
	src = gdImageScale(src,gdImageSX(src)*src_dim_percent,gdImageSY(src)*src_dim_percent);
	dst = gdImageCreateFromFile(filename_destination);
	gdImageCopy(dst,src,dst_x,dst_y,0,0,gdImageSX(src),gdImageSY(src));
	gdImagePng(dst,out);
	gdFree(src);
	gdFree(dst);
	fclose(out);
}
