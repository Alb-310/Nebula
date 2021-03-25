#include "gd.h"
#include <stdio.h>
#include <err.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "func_divers.h"
void Resize(char *filename, int width, int height)
{
	gdImagePtr im;
	FILE *out;
	out = fopen("result.png","wb");
	im=gdImageCreateFromFile(filename);
	im=gdImageScale(im,width,height);
	gdImagePng(im,out);
	gdFree(im);
	fclose(out);
}

void Crop(char *filename,int x, int y, int width,int height)
{
	gdImagePtr im;
	FILE *out;
	out = fopen("result.png","wb");
	im= gdImageCreateFromFile(filename);
	gdRect *crop=malloc(sizeof(gdRect));

	crop->x=x;
	crop->y=y;
	crop->width=width;
	crop->height=height;	
	im=gdImageCrop(im,crop);
	gdImagePng(im,out);
	gdFree(crop);
	gdFree(im);
	fclose(out);

}

void Rotate(char *filename,int angle)
{
	gdImagePtr im;
	FILE *out;
	out = fopen("result.png","wb");
	im=gdImageCreateFromFile(filename);
	im=gdImageRotateInterpolated(im,angle,0);
	if(im==NULL)
		errx(1,"rotation failure");
	gdImagePng(im,out);

	gdFree(im);
	fclose(out);
}

void Add_text(char *filename,char *font,int x,int y,int width,int height,int color,double size,double angle,char* text)
{
	gdImagePtr im;
	FILE *out;
	out = fopen("result.png","wb");
	im= gdImageCreateFromFile(filename);
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
	gdFree(im);
	fclose(out);
}
