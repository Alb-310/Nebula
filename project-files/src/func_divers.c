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

/*int main(int argc, char *argv[]){
	gdImagePtr im = gdImageCreateFromFile(argv[1]);
	FILE *out;
	char *font = "/usr/share/fonts/truetype/ubuntu/Ubuntu-LI.ttf";
	Add_text(im, out, "test.png", font, 10, 10+12, 200, 250, 0, 12, 0, "Test");
	return 0;
}*/
