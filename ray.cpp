#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

internal f32 RandomUnilateral(void)
{
    f32 Result = (f32)rand() / (f32)RAND_MAX;
    return Result;
}

internal f32 RandomBilateral(void)
{
    f32 Result =  2.0f*RandomUnilateral() - 1.0f;
    return Result;
}

internal v3
RayCast(world *World, v3 RayOrigin, v3 RayDirection)
{
    v3 Result =  {};
    v3 Attenuation = V3(1, 1, 1);

    f32 MinHitDistance = 0.001f;
    f32 Tolerance = 0.0001f;
    
   for(u32 RayCount = 0; RayCount < 8; RayCount++) {
        f32 HitDistance = FLT_MAX;
        u32 HitMatIndex = 0;
        v3 NextOrigin = {};
        v3 NextNormal = {};

        // Planes
        for (u32 PlaneIndex = 0; PlaneIndex < World->PlaneCount; PlaneIndex++) {
            plane Plane = World->Planes[PlaneIndex];
        
            f32 Denom = Dot(Plane.N, RayDirection); 
            if (Denom < -Tolerance || Denom > Tolerance) 
            {
                f32 ThisDistance = (-Plane.d - Dot(Plane.N, RayOrigin)) / Denom;
                if (ThisDistance > MinHitDistance && ThisDistance < HitDistance) {
                    HitDistance = ThisDistance;
                    HitMatIndex = Plane.MatIndex;

                    NextOrigin = RayOrigin + ThisDistance*RayDirection;
                    NextNormal = Plane.N;
                }
            }
        }
        
        // Speheres
        for (u32 SphereIndex = 0; SphereIndex < World->SphereCount; SphereIndex++) {
            sphere Sphere = World->Spheres[SphereIndex];

            v3 SphereRelativeOrigin = RayOrigin - Sphere.P;
            f32 a = Dot(RayDirection, RayDirection);
            f32 b = 2.0f * Dot(RayDirection, SphereRelativeOrigin);
            f32 c = Dot(SphereRelativeOrigin, SphereRelativeOrigin) - Sphere.r*Sphere.r;

            f32 RootTerm = Sqrt(b*b - 4.0f*a*c);
            f32 Denom = 2.0f * a;

            if (RootTerm > Tolerance)
            {
                f32 RootPos = (-b + RootTerm) / Denom;
                f32 RootNeg = (-b - RootTerm) / Denom;

                f32 ThisDistance = RootPos;
                if (RootNeg > MinHitDistance && RootNeg < RootPos) {
                    ThisDistance = RootNeg;
                }

                if (ThisDistance > MinHitDistance && ThisDistance < HitDistance) {
                    HitDistance = ThisDistance;
                    HitMatIndex = Sphere.MatIndex;

                    NextOrigin = RayOrigin + ThisDistance*RayDirection;
                    NextNormal = NOZ(NextOrigin - Sphere.P);
                }
            }
        }

        if (HitMatIndex) 
        {
            material Mat = World->Materials[HitMatIndex];
            Result += Hadamard(Attenuation, Mat.EmitColor); // Hadamard is essentially multiply

            //f32 CosAtten = Dot(-RayDirection, NextNormal);
            //if (CosAtten < 0) { CosAtten = 0; } 
            //Attenuation = Hadamard(CosAtten, Mat.ReflColor);

            Attenuation = Hadamard(Attenuation, Mat.ReflColor);

            RayOrigin = NextOrigin;
            // TODO: reflections
            v3 PureBounce = RayDirection - 2.0f*Dot(RayDirection, NextNormal)*NextNormal;
            v3 RandomBounce = NOZ(NextNormal + V3(RandomBilateral(), RandomBilateral(), RandomBilateral()));
            RayDirection = NOZ(Lerp(RandomBounce, PureBounce, Mat.Specular));
        } 
        else 
        {
            material Mat = World->Materials[HitMatIndex];
            Result += Hadamard(Attenuation, Mat.EmitColor);
         
            break;
        }

    }

    return Result;
}

internal f32
ExactLinearTosRGB(f32 L)
{
   if (L < 0.0f) { L = 0.0f; }
   if (L > 1.0f) { L = 1.0f; }

   f32 S = L*12.92f;
   if (L > 0.0031308f) {
       S = 1.055f*pow(L, 1.0f/2.4f) - 0.055f;
   }

   return S;
}

int main(int argc, char** argv) 
{
    material Materials[7] = {};
    //Materials[0].EmitColor = V3(212/255.0f, 102/255.0f, 197/255.0f);
    //Materials[1].ReflColor = V3(17/255.0f, 1/255.0f, 50/255.0f);
    //Materials[2].ReflColor = V3(227/255.0f, 174/255.0f, 59/255.0f);

    Materials[0].EmitColor = V3(0.3f, 0.4f, 0.5f);
    Materials[1].ReflColor = V3(0.5f, 0.5f, 0.5f);
    Materials[2].ReflColor = V3(0.7f, 0.5f, 0.3f);
    Materials[3].EmitColor = V3(4.0f, 1.5f, 0.0f);
    Materials[4].ReflColor = V3(0.2f, 0.8f, 0.2f);
    Materials[4].Specular = 0.7f;
    Materials[5].ReflColor = V3(0.4f, 0.8f, 0.9f);
    Materials[5].Specular = 0.85f;
    Materials[6].ReflColor = V3(0.95f, 0.95f, 0.95f);
    Materials[6].Specular = 1.0f;

    plane Planes[1] = {};
    Planes[0].N = V3(0,0,1);
    Planes[0].d = 0;
    Planes[0].MatIndex = 1; 

    sphere Spheres[5] = {};
    Spheres[0].P = V3(0, 0, 0);
    Spheres[0].r = 1.0f;
    Spheres[0].MatIndex = 2;
    Spheres[1].P = V3(3, -2, 1);
    Spheres[1].r = 1.0f;
    Spheres[1].MatIndex = 3;
    Spheres[2].P = V3(-2, -1, 2);
    Spheres[2].r = 1.0f;
    Spheres[2].MatIndex = 4;
    Spheres[3].P = V3(1, -1, 3);
    Spheres[3].r = 1.0f;
    Spheres[3].MatIndex = 5;
    Spheres[4].P = V3(-2, 3, 0);
    Spheres[4].r = 1.0f;
    Spheres[4].MatIndex = 6;

    world World = {};
    World.MaterialCount = ArrayCount(Materials);
    World.Materials = Materials;
    World.PlaneCount = ArrayCount(Planes);
    World.Planes = Planes;
    World.SphereCount = ArrayCount(Spheres);
    World.Spheres = Spheres;

    image_u32 Image = AllocateImage(1280, 720);

    v3 CameraP = V3(0, -10, 1);
    v3 CameraZ = NOZ(CameraP);
    v3 CameraX = NOZ(Cross(V3(0,0,1), CameraZ));
    v3 CameraY = NOZ(Cross(CameraZ, CameraX));

    f32 FilmDist = 1.0f;

    f32 FilmW = 1.0f;
    f32 FilmH = 1.0f;

    if (Image.Width > Image.Height) {
        FilmH = FilmW * ((f32)Image.Height / (f32)Image.Width);
    } else if (Image.Height > Image.Width) {
        FilmW = FilmH * ((f32)Image.Width / (f32)Image.Height);
    }

    f32 HalfFilmW = 0.5f*FilmW;
    f32 HalfFilmH = 0.5f*FilmH;
    v3 FilmCenter = CameraP - FilmDist*CameraZ;

    f32 HalfPixW = 0.5f / Image.Width;
    f32 HalfPixH = 0.5f / Image.Height;

    u32 RaysPerPixel = 256;
    f32 Contrib = 1.0f / RaysPerPixel;

    // write pixels to image
    u32 *Out = Image.Pixels;
    for (u32 y = 0; y < Image.Height; y++) {
        f32 FilmRatioY = 2.0f * ((f32)y / (f32)Image.Height) - 1.0f;
        for (u32 x = 0; x < Image.Width; x++) {
            f32 FilmRatioX = 2.0f * ((f32)x / (f32)Image.Width) - 1.0f;

            v3 Color = {};
            for (u32 RayIndex = 0; RayIndex < RaysPerPixel; RayIndex++) {
                f32 OffX = FilmRatioX + RandomBilateral() * HalfPixW;
                f32 OffY = FilmRatioY + RandomBilateral() * HalfPixH;

                v3 FilmP = FilmCenter + OffX*HalfFilmW*CameraX + OffY*HalfFilmH*CameraY;

                v3 RayOrigin = CameraP;
                v3 RayDirection = NOZ(FilmP - CameraP);


                Color += Contrib*RayCast(&World, RayOrigin, RayDirection);
            }


            //u32 BMPColor = BGRAPack(V4(Color*255.0f, 255.0f));

            v4 BMPColor = 
            {
                255.0f*ExactLinearTosRGB(Color.r),
                255.0f*ExactLinearTosRGB(Color.g),
                255.0f*ExactLinearTosRGB(Color.b),
                255.0f,
            };

            u32 BMPValue = BGRAPack(BMPColor);

            *Out++ = BMPValue;
        }

        if ((y % 64) == 0) {
            printf("\rRaycastin' %d%%...   ", 100*y / Image.Height);
            fflush(stdout);
        }
    }

    WriteImage(Image, "test.bmp");
    printf("\nDone.\n");

    return 0;
}

