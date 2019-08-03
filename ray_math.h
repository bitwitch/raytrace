#include <xmmintrin.h>

inline f32 Square(f32 A)
{
    f32 Result = A*A;
    return Result;
}

inline f32 Sqrt(f32 X) 
{
    f32 Result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(X)));
    return Result;
}

union v2 
{
    struct {
        f32 x, y;
    };
    struct {
        f32 u, v;
    };
    f32 E[2];
};

union v3
{
    struct {
        f32 x, y, z;
    };
    struct {
        f32 u, v, __;
    };
    struct {
        f32 r, g, b;
    };
    struct {
        v2 xy;
        f32 Ignored0_;
    };
    struct {
        f32 Ignored1_;
        v2 yz;
    };
    struct {
        v2 uv;
        f32 Ignored2_;
    };
    struct {
        f32 Ignored3_;
        v2 v__;
    };
    f32 E[3];
};

inline v2
V2(f32 X, f32 Y) 
{
    v2 Result;
    Result.x = X;
    Result.y = Y;
    return Result;
}

//
// v3 operations
//

inline v3
operator*(f32 A, v3 B)
{
    v3 Result;
    Result.x = A*B.x;
    Result.y = A*B.y;
    Result.z = A*B.z;
    return Result;
}

inline v3
operator*(v3 B, f32 A)
{
    v3 Result = A*B;
    return Result;
}

inline v3
V3(f32 X, f32 Y, f32 Z) 
{
    v3 Result;
    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    return Result;
}

inline f32
Dot(v3 A, v3 B)
{
    f32 Result = A.x*B.x + A.y*B.y + A.z*B.z;
    return Result;
}

inline v3
Cross(v3 A, v3 B)
{
    v3 Result;
    Result.x = A.y*B.z - A.z*B.y;
    Result.y = A.z*B.x - A.x*B.z;
    Result.z = A.x*B.y - A.y*B.x;
    return Result;
}

inline f32
LengthSq(v3 A)
{
   f32 Result = Dot(A, A); 
   return Result;
}

inline f32
Length(v3 A)
{
    f32 Result = Sqrt(LengthSq(A));    
    return Result;
}

inline v3
Normalize(v3 A)
{
    v3 Result = A * (1.0f / Length(A));
    return Result;
}

inline v3
NOZ(v3 A)
{
    v3 Result = {};
    f32 LenSq = LengthSq(A);
    if (LenSq > Square(0.0001f)) {
        Result = A * (1.0f / Length(A));
    }
    return Result;
}


