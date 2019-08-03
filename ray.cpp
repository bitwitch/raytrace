#include <stdio.h>
#include <stdlib.h>
#include <float.h>
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

internal v3
RayCast(world *World, v3 RayOrigin, v3 RayDirection)
{
    v3 Result = World->Materials[0].Color;

    f32 HitDistance = FLT_MAX;

    return Result;
}

int main(int argc, char** argv) 
{
    material Materials[2] = {};
    Materials[0].Color = V3(0,0,0);
    Materials[1].Color = V3(0,0,1);

    plane Plane = {};
    Plane.N = V3(0,0,1);
    Plane.d = 0;
    Plane.MatIndex = 1; 

    sphere Spheres[5] = {};

    world World = {};
    World.SphereCount = 5;
    World.Spheres = Spheres;
    World.PlaneCount = 1;
    World.Planes = &Plane;
    World.MaterialCount = 2;
    World.Materials = Materials;

    u32 OutputWidth = 1280;
    u32 OutputHeight = 720;

    image_u32 Image = AllocateImage(OutputWidth, OutputHeight);

    v3 CameraP = V3(0, 10, 2);
    v3 CameraZ = NOZ(CameraP);
    v3 CameraX = NOZ(Cross(CameraZ, V3(0,0,1)));
    v3 CameraY = NOZ(Cross(CameraZ, CameraX));

    f32 FilmDist = 1.0f;
    f32 FilmW = 1.0f;
    f32 FilmH = 1.0f;
    f32 HalfFilmW = 0.5f*FilmW;
    f32 HalfFilmH = 0.5f*FilmH;
    v3 FilmCenter = CameraP - FilmDist*CameraZ;

    // write pixels to image
    u32 *Out = Image.Pixels;
    for (u32 y = 0; y < OutputHeight; y++) {
        f32 FilmRatioY = 2.0f * ((f32)y / (f32)Image.Height) - 1.0f;
        for (u32 x = 0; x < OutputWidth; x++) {
            f32 FilmRatioX = 2.0f * ((f32)x / (f32)Image.Width) - 1.0f;
            v3 FilmP = FilmCenter + FilmRatioX*HalfFilmW*CameraX + FilmRatioY*HalfFilmH*CameraY;

            v3 RayOrigin = CameraP;
            v3 RayDirection = NOZ(FilmP - CameraP);
            
            v3 Color = RayCast(&World, RayOrigin, RayDirection);

            u32 BMPColor = BGRAPack(V4(Color*255.0f, 255.0f));

            *Out++ = BMPColor;
        }
    }

    WriteImage(Image, "test.bmp");

    return 0;
}

