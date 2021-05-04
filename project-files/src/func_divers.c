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

void picture_insertion(gdImagePtr src, FILE *out, gdImagePtr dst, char *path, int dst_x,int dst_y, float src_dim_percent)
{	
	out = fopen(path, "wb");
	src = gdImageScale(src,gdImageSX(src)*src_dim_percent, gdImageSY(src)*src_dim_percent);
	int width = gdImageSX(src);
	int height = gdImageSY(src);
	gdImageCopy(dst, src, dst_x - (width/2), dst_y - (height/2), 0, 0, gdImageSX(src), gdImageSY(src));
	gdImagePng(dst,out);
	fclose(out);
}

void add_motif(gdImagePtr dst, FILE *out, char* path, char *motif,int dst_x,int dst_y,float src_dim_percent)
{
	gdImagePtr src;
	char *filename_source;
	out = fopen(path,"wb");
	if(strcmp("square",motif)==0)
		filename_source="src/resources/func_divers/square.png";
	else if(strcmp("circle",motif)==0)
		filename_source="src/resources/func_divers/circle.png";
	else if(strcmp("triangle",motif)==0)
		filename_source="src/resources/func_divers/triangle.png";
	else if(strcmp("pentagon",motif)==0)
		filename_source="src/resources/func_divers/pentagon.png";
	else if(strcmp("star",motif)==0)
		filename_source="src/resources/func_divers/star.png";
	else if(strcmp("heart",motif)==0)
		filename_source="src/resources/func_divers/heart.png";
	else
		errx(1,"wrong motif argument given");
	src = gdImageCreateFromFile(filename_source);
	src = gdImageScale(src,gdImageSX(src)*src_dim_percent,gdImageSY(src)*src_dim_percent);
	int width = gdImageSX(src);
	int height = gdImageSY(src);
	gdImageCopy(dst,src,dst_x - (width/2), dst_y - (height/2),0,0,gdImageSX(src),gdImageSY(src));
	gdImagePng(dst,out);
	fclose(out);
}