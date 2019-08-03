#define internal static;

typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32; 

typedef char i8;
typedef short i16;
typedef int i32; 

typedef float f32;

#include "ray_math.h"

#pragma pack(push, 1)
struct bitmap_header {
    u16 FileType;
    u32 FileSize;
    u16 Reserved1;
    u16 Reserved2;
    u32 BitmapOffset;
    u32 Size;
    i32 Width;
    i32 Height;
    u16 Planes;
    u16 BitsPerPixel;
    u32 Compression;
    u32 SizeOfBitmap;
    i32 HorzResolution;
    i32 VertResolution;
    u32 ColorsUsed;
    u32 ColorsImportant;
}; 
#pragma pack(pop)

struct image_u32 {
    u32 Width;
    u32 Height;
    u32 *Pixels;
};

struct material {
    v3 Color;
};

struct sphere {
    v3 P;
    f32 r;
    u32 MatIndex;
};

struct plane {
    v3 N;
    f32 d;
    u32 MatIndex;
};

struct world {
    u32 SphereCount;
    sphere *Spheres;

    u32 PlaneCount;
    plane *Planes;

    u32 MaterialCount;
    material *Materials;
};











