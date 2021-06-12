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
                        double angle, const char* text)
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
	printf("1\n");
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

	printf("2\n");
	src = gdImageCreateFromFile(filename_source);
	src = gdImageScale(src,gdImageSX(src)*src_dim_percent,gdImageSY(src)*src_dim_percent);
	int width = gdImageSX(src);
	int height = gdImageSY(src);
	gdImageCopy(dst,src,dst_x - (width/2), dst_y - (height/2),0,0,gdImageSX(src),gdImageSY(src));
	gdImagePng(dst,out);
	fclose(out);
}

void collage(char *filename_background, char *filename_first_picture, 
			 char *filename_second_picture, char *filename_third_picture,
			 char *filename_fourth_picture, char *dst_path)
{
	if(strcmp(filename_third_picture,"")==0 && strcmp(filename_fourth_picture,"")== 0)
	{
		gdImagePtr background;
		gdImagePtr first_pic;
		gdImagePtr second_pic;
		FILE *out;
		out = fopen(dst_path,"wb");
		background = gdImageCreateFromFile(filename_background);
		background = gdImageScale(background,1920,1080);
		first_pic = gdImageCreateFromFile(filename_first_picture);
		first_pic = gdImageScale(first_pic,1920,1080);
		second_pic = gdImageCreateFromFile(filename_second_picture);
		second_pic = gdImageScale(second_pic,1920,1080);

		gdRect *crop=malloc(sizeof(gdRect));
		crop->x=(1920/2)-(1920/4);
		crop->y=10;
		crop->width=1920/2-20;
		crop->height=1060;
		first_pic=gdImageCrop(first_pic,crop);	
		second_pic=gdImageCrop(second_pic,crop);

		gdImageCopy(background,first_pic,10,10,0,0,gdImageSX(first_pic),gdImageSY(first_pic));
		gdImageCopy(background,second_pic,1920/2+5,10,0,0,gdImageSX(second_pic),gdImageSY(second_pic));
		gdImagePng(background,out);
		gdFree(background);
		gdFree(first_pic);
		gdFree(second_pic);
		fclose(out);
	}
	else if(strcmp(filename_third_picture,"")!=0 && strcmp(filename_fourth_picture,"")==0)
	{
		gdImagePtr background;
		gdImagePtr first_pic;
		gdImagePtr second_pic;
		gdImagePtr third_pic;
		FILE *out;
		out = fopen(dst_path,"wb");
		background = gdImageCreateFromFile(filename_background);
		background = gdImageScale(background,1920,1080);
		first_pic = gdImageCreateFromFile(filename_first_picture);
		first_pic = gdImageScale(first_pic,1920,1080);
		second_pic = gdImageCreateFromFile(filename_second_picture);
		second_pic = gdImageScale(second_pic,1920,1080);
		third_pic = gdImageCreateFromFile(filename_third_picture);
		third_pic = gdImageScale(third_pic,1920,1080);
		gdRect *crop=malloc(sizeof(gdRect));
		crop->x=(1920/2)-(1920/6);
		crop->y=10;
		crop->width=1920/3-20;
		crop->height=1060;
		first_pic=gdImageCrop(first_pic,crop);	
		second_pic=gdImageCrop(second_pic,crop);
		third_pic=gdImageCrop(third_pic,crop);
		gdImageCopy(background,first_pic,10,10,0,0,gdImageSX(first_pic),gdImageSY(first_pic));
		gdImageCopy(background,second_pic,1920/3+5,10,0,0,gdImageSX(second_pic),gdImageSY(second_pic));
		gdImageCopy(background,third_pic,1920*2/3+5,10,0,0,gdImageSX(third_pic),gdImageSY(third_pic));
		gdImagePng(background,out);
		gdFree(background);
		gdFree(first_pic);
		gdFree(second_pic);
		gdFree(third_pic);
		fclose(out);

	}
	else
	{
		gdImagePtr background;
		gdImagePtr first_pic;
		gdImagePtr second_pic;
		gdImagePtr third_pic;
		gdImagePtr fourth_pic;
		FILE *out;
		out = fopen(dst_path,"wb");
		background = gdImageCreateFromFile(filename_background);
		background = gdImageScale(background,1920,1080);
		first_pic = gdImageCreateFromFile(filename_first_picture);
		first_pic = gdImageScale(first_pic,1920,1080);
		second_pic = gdImageCreateFromFile(filename_second_picture);
		second_pic = gdImageScale(second_pic,1920,1080);
		third_pic = gdImageCreateFromFile(filename_third_picture);
		third_pic = gdImageScale(third_pic,1920,1080);
		fourth_pic = gdImageCreateFromFile(filename_fourth_picture);
		fourth_pic = gdImageScale(fourth_pic,1920,1080);
		gdRect *crop=malloc(sizeof(gdRect));
		crop->x=(1920/2)-(1920/8);
		crop->y=10;
		crop->width=1920/4-20;
		crop->height=1060;
		first_pic=gdImageCrop(first_pic,crop);	
		second_pic=gdImageCrop(second_pic,crop);
		third_pic=gdImageCrop(third_pic,crop);
		fourth_pic=gdImageCrop(fourth_pic,crop);
		gdImageCopy(background,first_pic,10,10,0,0,gdImageSX(first_pic),gdImageSY(first_pic));
		gdImageCopy(background,second_pic,1920/4+5,10,0,0,gdImageSX(second_pic),gdImageSY(second_pic));
		gdImageCopy(background,third_pic,1920*2/4+5,10,0,0,gdImageSX(third_pic),gdImageSY(third_pic));
		gdImageCopy(background,fourth_pic,1920*3/4+5,10,0,0,gdImageSX(fourth_pic),gdImageSY(fourth_pic));
		gdImagePng(background,out);
		gdFree(background);
		gdFree(first_pic);
		gdFree(second_pic);
		gdFree(third_pic);
		gdFree(fourth_pic);
		fclose(out);

	}
}

void collage_2(char *filename_background, char *filename_first_picture,
			   char *filename_second_picture, char *filename_third_picture,
			   char *filename_fourth_picture, char *dst_path)
{
	if(strcmp(filename_third_picture,"")==0 && strcmp(filename_fourth_picture,"")== 0)
	{
		gdImagePtr background;
		gdImagePtr first_pic;
		gdImagePtr second_pic;
		FILE *out;
		out = fopen(dst_path,"wb");
		background = gdImageCreateFromFile(filename_background);
		background = gdImageScale(background,1920,1080);
		first_pic = gdImageCreateFromFile(filename_first_picture);
		first_pic = gdImageScale(first_pic,1920,1080);
		second_pic = gdImageCreateFromFile(filename_second_picture);
		second_pic = gdImageScale(second_pic,1920,1080);

		gdRect *crop=malloc(sizeof(gdRect));
		crop->x=10;	//(1920/2)-(1920/4);
		crop->y=(1080/2)-(1080/4);	//10;
		crop->width=1900;	//1920/2-20;
		crop->height=1080/2-20;	//1060;
		first_pic=gdImageCrop(first_pic,crop);	
		second_pic=gdImageCrop(second_pic,crop);

		gdImageCopy(background,first_pic,10,10,0,0,gdImageSX(first_pic),gdImageSY(first_pic));
		gdImageCopy(background,second_pic,10,1080/2+5,0,0,gdImageSX(second_pic),gdImageSY(second_pic));
		gdImagePng(background,out);
		gdFree(crop);
		gdFree(background);
		gdFree(first_pic);
		gdFree(second_pic);
		fclose(out);
	}
	else if(strcmp(filename_third_picture,"")!=0 && strcmp(filename_fourth_picture,"")==0)
	{
		gdImagePtr background;
		gdImagePtr first_pic;
		gdImagePtr second_pic;
		gdImagePtr third_pic;
		FILE *out;
		out = fopen(dst_path,"wb");
		background = gdImageCreateFromFile(filename_background);
		background = gdImageScale(background,1920,1080);
		first_pic = gdImageCreateFromFile(filename_first_picture);
		first_pic = gdImageScale(first_pic,1920,1080);
		second_pic = gdImageCreateFromFile(filename_second_picture);
		second_pic = gdImageScale(second_pic,1920,1080);
		third_pic = gdImageCreateFromFile(filename_third_picture);
		third_pic = gdImageScale(third_pic,1920,1080);
		gdRect *crop=malloc(sizeof(gdRect));
		crop->x=10; //(1920/2)-(1920/6);
		crop->y=(1080/2)-(1080/6);
		crop->width=1900;//1920/3-20;
		crop->height=1080/3-20;
		first_pic=gdImageCrop(first_pic,crop);	
		second_pic=gdImageCrop(second_pic,crop);
		third_pic=gdImageCrop(third_pic,crop);
		gdImageCopy(background,first_pic,10,10,0,0,gdImageSX(first_pic),gdImageSY(first_pic));
		gdImageCopy(background,second_pic,10,1080/3+5,0,0,gdImageSX(second_pic),gdImageSY(second_pic));
		gdImageCopy(background,third_pic,10,1080*2/3+5,0,0,gdImageSX(third_pic),gdImageSY(third_pic));
		gdImagePng(background,out);
		gdFree(crop);
		gdFree(background);
		gdFree(first_pic);
		gdFree(second_pic);
		gdFree(third_pic);
		fclose(out);

	}
	else
	{
		gdImagePtr background;
		gdImagePtr first_pic;
		gdImagePtr second_pic;
		gdImagePtr third_pic;
		gdImagePtr fourth_pic;
		FILE *out;
		out = fopen(dst_path,"wb");
		background = gdImageCreateFromFile(filename_background);
		background = gdImageScale(background,1920,1080);
		first_pic = gdImageCreateFromFile(filename_first_picture);
		first_pic = gdImageScale(first_pic,1920,1080);
		second_pic = gdImageCreateFromFile(filename_second_picture);
		second_pic = gdImageScale(second_pic,1920,1080);
		third_pic = gdImageCreateFromFile(filename_third_picture);
		third_pic = gdImageScale(third_pic,1920,1080);
		fourth_pic = gdImageCreateFromFile(filename_fourth_picture);
		fourth_pic = gdImageScale(fourth_pic,1920,1080);
		gdRect *crop=malloc(sizeof(gdRect));
		crop->x=10;//(1920/2)-(1920/8);
		crop->y=(1080/2)-(1080/8);
		crop->width=1900;//1920/4-20;
		crop->height=1080/4-20;
		first_pic=gdImageCrop(first_pic,crop);	
		second_pic=gdImageCrop(second_pic,crop);
		third_pic=gdImageCrop(third_pic,crop);
		fourth_pic=gdImageCrop(fourth_pic,crop);
		gdImageCopy(background,first_pic,10,10,0,0,gdImageSX(first_pic),gdImageSY(first_pic));
		gdImageCopy(background,second_pic,10,1080/4+5,0,0,gdImageSX(second_pic),gdImageSY(second_pic));
		gdImageCopy(background,third_pic,10,1080*2/4+5,0,0,gdImageSX(third_pic),gdImageSY(third_pic));
		gdImageCopy(background,fourth_pic,10,1080*3/4+5,0,0,gdImageSX(fourth_pic),gdImageSY(fourth_pic));
		gdImagePng(background,out);
		gdFree(crop);
		gdFree(background);
		gdFree(first_pic);
		gdFree(second_pic);
		gdFree(third_pic);
		gdFree(fourth_pic);
		fclose(out);

	}
}

void collage_3(char *filename_background, char *filename_first_picture,
			   char *filename_second_picture, char *filename_third_picture,
			   char *filename_fourth_picture, char *dst_path)
{
	gdImagePtr background;
	gdImagePtr first_pic;
	gdImagePtr second_pic;
	gdImagePtr third_pic;
	gdImagePtr fourth_pic;
	FILE *out;
	out = fopen(dst_path,"wb");
	background = gdImageCreateFromFile(filename_background);
	background = gdImageScale(background,1920,1080);
	first_pic = gdImageCreateFromFile(filename_first_picture);
	first_pic = gdImageScale(first_pic,1920,1080);
	second_pic = gdImageCreateFromFile(filename_second_picture);
	second_pic = gdImageScale(second_pic,1920,1080);
	third_pic = gdImageCreateFromFile(filename_third_picture);
	third_pic = gdImageScale(third_pic,1920,1080);
	fourth_pic = gdImageCreateFromFile(filename_fourth_picture);
	fourth_pic = gdImageScale(fourth_pic,1920,1080);
	gdRect *crop=malloc(sizeof(gdRect));
	crop->x=(1920/2)-(1920/4);
	crop->y=(1080/2)-(1080/4);
	crop->width=1920/2-20;
	crop->height=1080/2-20;
	first_pic=gdImageCrop(first_pic,crop);
	second_pic=gdImageCrop(second_pic,crop);
	third_pic=gdImageCrop(third_pic,crop);
	fourth_pic=gdImageCrop(fourth_pic,crop);
	gdImageCopy(background,first_pic,10,10,0,0,gdImageSX(first_pic),gdImageSY(first_pic));
	gdImageCopy(background,second_pic,1920/2+5,10,0,0,gdImageSX(second_pic),gdImageSY(second_pic));
	gdImageCopy(background,third_pic,10,1080/2+5,0,0,gdImageSX(third_pic),gdImageSY(third_pic));
	gdImageCopy(background,fourth_pic,1920/2+5,1080/2+5,0,0,gdImageSX(fourth_pic),gdImageSY(fourth_pic));
	gdImagePng(background,out);
	gdFree(crop);
	gdFree(background);
	gdFree(first_pic);
	gdFree(second_pic);
	gdFree(third_pic);
	gdFree(fourth_pic);
	fclose(out);
}

void collage_4(char *filename_background, char *filename_first_picture,
			   char *filename_second_picture, char *filename_third_picture,
			   char *dst_path)
{
	gdImagePtr background;
	gdImagePtr first_pic;
	gdImagePtr second_pic;
	gdImagePtr third_pic;
	FILE *out;
	out = fopen(dst_path,"wb");
	background = gdImageCreateFromFile(filename_background);
	background = gdImageScale(background,1920,1080);
	first_pic = gdImageCreateFromFile(filename_first_picture);
	first_pic = gdImageScale(first_pic,1920,1080);
	second_pic = gdImageCreateFromFile(filename_second_picture);
	second_pic = gdImageScale(second_pic,1920,1080);
	third_pic = gdImageCreateFromFile(filename_third_picture);
	third_pic = gdImageScale(third_pic,1920,1080);

	gdRect *crop_1=malloc(sizeof(gdRect));
	crop_1->x=(1920/2)-(1920/4);
	crop_1->y=(1080/2)-(1080/4);
	crop_1->width=1920/2-20;
	crop_1->height=1080/2-20;

	gdRect *crop_2=malloc(sizeof(gdRect));
	crop_2->x=(1920/2)-(1920/4);
	crop_2->y=10;
	crop_2->width=1920/2-20;
	crop_2->height = 1060;

	first_pic=gdImageCrop(first_pic,crop_2);
	second_pic=gdImageCrop(second_pic,crop_1);
	third_pic=gdImageCrop(third_pic,crop_1);
	gdImageCopy(background,first_pic,10,10,0,0,gdImageSX(first_pic),gdImageSY(first_pic));
	gdImageCopy(background,second_pic,1920/2+5,10,0,0,gdImageSX(second_pic),gdImageSY(second_pic));
	gdImageCopy(background,third_pic,1920/2+5,1080/2+5,0,0,gdImageSX(third_pic),gdImageSY(third_pic));
	gdImagePng(background,out);
	gdFree(crop_1);
	gdFree(crop_2);
	gdFree(background);
	gdFree(first_pic);
	gdFree(second_pic);
	gdFree(third_pic);
	fclose(out);
}

