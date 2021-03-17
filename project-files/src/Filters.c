#include <stdio.h>
#include "gd.h"
#include "Filters.h"

void OldSchool_Filter(gdImagePtr image, FILE *Output )
{
    Output = fopen ("temp.png", "wb");
    gdImageGrayScale(image);
    gdImagePng(image, Output);
    fclose (Output);
}

void Glow_Filter(gdImagePtr image, FILE *Output )
{
    Output = fopen ("temp.png", "wb");
    gdImageNegate(image);
    gdImagePng(image, Output);
    fclose (Output);
}

void RedFlag_Filter(gdImagePtr image, FILE *Output )
{
    Output = fopen ("temp.png", "wb");
    for (int y = 0; y < gdImageSY(image); y++)
    {
        for (int x = 0; x < gdImageSX(image); x++)
        {
            int pixel = gdImageGetPixel(image,x,y);
            int truepixel = gdImageGetTrueColorPixel (image,x,y);
            int r = gdImageRed(image,truepixel);
            int b = gdImageBlue(image,truepixel);
            int g = gdImageGreen(image,truepixel);
            if ( r>= 120 && b <= 60 && g <= 60)
            {
                continue;
            }
            else if (b == 0 && g == 0)
            {
                continue;
            }
            else if (r < 256 && b <= 10 && g <= 10)
            {
                continue;
            }
            
            else
            {
                r = g = b = (int) (.299 * r + .587 * g + .114 * b);

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
        
    }

    gdImagePng(image, Output);
    fclose (Output);
    
}


void SummerTime_Filter(gdImagePtr image, FILE *Output )
{
    Output = fopen ("temp.png", "wb");
    gdImageColor(image,20,0,0,0);
    gdImageBrightness(image,20);
    gdImageContrast(image,-20.2);
    gdImagePng(image, Output);
    fclose (Output);
}
