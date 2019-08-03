#include <stdio.h>
#include <stdlib.h>
#include "ray.h"

internal u32 
GetTotalPixelSize(image_u32 Image) 
{
    u32 Result = Image.Width * Image.Height * sizeof(u32);
    return Result;
}

internal image_u32 
AllocateImage(u32 Width, u32 Height) 
{
    image_u32 Image = {};
    Image.Width = Width;
    Image.Height = Height;

    u32 OutputPixelSize = GetTotalPixelSize(Image);
    Image.Pixels = (u32 *)malloc(OutputPixelSize);

    return Image;
}

internal void 
WriteImage(image_u32 Image, char *OutputFileName) 
{
    u32 OutputPixelSize = GetTotalPixelSize(Image); 

    bitmap_header Header = {};    
    Header.FileType = 0x4D42;
    Header.FileSize = sizeof(Header) + OutputPixelSize; // total filesize
    Header.BitmapOffset = sizeof(Header);
    Header.Size = sizeof(Header) - 14; // Number of bytes in the DIB header only
    Header.Width = Image.Width;
    Header.Height = Image.Height;
    Header.Planes = 1;
    Header.BitsPerPixel = 32;
    Header.Compression = 0;
    Header.SizeOfBitmap = OutputPixelSize;
    Header.HorzResolution = 0;
    Header.VertResolution = 0;
    Header.ColorsUsed = 0;
    Header.ColorsImportant = 0;   

    FILE *OutFile = fopen(OutputFileName, "wb");
    fwrite(&Header, sizeof(Header), 1, OutFile);
    fwrite(Image.Pixels, OutputPixelSize, 1, OutFile);
    fclose(OutFile);
}

int main(int argc, char** argv) 
{
    u32 OutputWidth = 1280;
    u32 OutputHeight = 720;

    image_u32 Image = AllocateImage(OutputWidth, OutputHeight);

    // write pixels to image
    u32 *Out = Image.Pixels;
    for (u32 y = 0; y < OutputHeight; y++) {
        for (u32 x = 0; x < OutputWidth; x++) {
            *Out++ = y < 32 ? 0XFFA500 : 0xFF00FF00;
        }
    }

    WriteImage(Image, "test.bmp");

    return 0;
}

