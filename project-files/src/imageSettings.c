#include <stdio.h>
#include "gd.h"
#include "imageSettings.h"



void Temperature(gdImagePtr image, int value, FILE *Output )
{
    Output = fopen ("temp.png", "wb");
    
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
    Output = fopen ("temp.png", "wb");
    
    gdImageBrightness(image,value);
    
    gdImagePng(image, Output);
    fclose (Output);
}

void Contrast(gdImagePtr image, double value, FILE *Output )
{
    Output = fopen ("temp.png", "wb");
    
    gdImageContrast(image,-value);
    
    gdImagePng(image, Output);
    fclose (Output);
}