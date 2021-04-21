#include <stdio.h>
#include "gd.h"
#include ".h/image_settings.h"

void Temperature(gdImagePtr image, int value, FILE *Output )
{
    Output = fopen ("cache/temp_img.png", "wb");
    
    if (value>0)
    {
        gdImageColor(image, value, 0, 0, 0);
    }
    else
    {
        gdImageColor(image, 0, 0, -value, 0);
    }
    
    gdImagePng(image, Output);
    fclose (Output);
}

void Brightness(gdImagePtr image, int value, FILE *Output )
{
    Output = fopen ("cache/temp_img.png", "wb");
    
    gdImageBrightness(image,value);
    
    gdImagePng(image, Output);
    fclose (Output);
}

void Contrast(gdImagePtr image, double value, FILE *Output )
{
    Output = fopen ("cache/temp_img.png", "wb");
    
    gdImageContrast(image,-value);
    
    gdImagePng(image, Output);
    fclose (Output);
}

void Noise (gdImagePtr image, int value, FILE *Output )
{
    Output = fopen ("cache/temp.png", "wb");

    for (int x = 0; x < image->sx; ++x)
    {
        for (int y = 0; y < image->sy; ++y)
        {
            int pixel = gdImageGetPixel(image,x,y);
            int truepixel = gdImageGetTrueColorPixel (image,x,y);
            int r = gdImageRed(image,truepixel);
            int b = gdImageBlue(image,truepixel);
            int g = gdImageGreen(image,truepixel);
            r += rand() % (value + 1 - (-value)) + (-value);
            b += rand() % (value + 1 - (-value)) + (-value);
            g += rand() % (value + 1 - (-value)) + (-value);
            if (r>255){r = 255;}
            if (b>255){b = 255;}
            if (g>255){g = 255;}
            if (r < 0){r = 0;}
            if (b < 0){b = 0;}
            if (g < 0){g = 0;}
            int new_pxl = gdImageColorAllocateAlpha(image,
                 r, g, b, gdImageAlpha(image,truepixel));
			if (new_pxl == -1) 
            {
				new_pxl = gdImageColorClosestAlpha(image,
                     r, g, b, gdImageAlpha(image,truepixel));
			}
			gdImageSetPixel (image, x, y, new_pxl);
        }
    }
    
    gdImagePng(image, Output);
    fclose (Output);
}

//BETA FONCTION
void Sharpen(gdImagePtr image, double value, FILE *Output )
{
    /* Fonction a test: Value doit etre entre 0.5 et 1 pour qu'il y'est un changement acceptable et si plus ou moin: aucun changement */
    Output = fopen ("temp.png", "wb");
    
    float filter[3][3] =	{
        {0.0,-1.0,0.0},
		{-1.0,5.0,-1.0},
		{0.0,-1.0,0.0}};

    if (value > 1 || value < 0,5)
    {
        gdImagePng(image, Output);
        fclose (Output);
        return;
    }
    
	if(gdImageConvolution(image, filter, value, 0)==0)
    {
        printf("error\n");
    }
    
    gdImagePng(image, Output);
    fclose (Output);
}
