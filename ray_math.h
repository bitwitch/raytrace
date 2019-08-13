#include <xmmintrin.h>

// Intrinsics
inline u32
RoundF32ToU32(f32 Real32)
{
    u32 Result = (u32)_mm_cvtss_si32(_mm_set_ss(Real32));
    return Result;
}

inline f32
Sqrt(f32 X) 
{
    f32 Result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(X)));
    return Result;
}

// Scalar operations
inline f32
Square(f32 A)
{
    f32 Result = A*A;
    return Result;
}

inline f32
LinearTosRGB(f32 L)
{
   if (L < 0.0f) { L = 0.0f; }
   if (L > 1.0f) { L = 1.0f; }

   f32 S = L*12.92f;
   if (L > 0.0031308f) {
       S = 1.055f*pow(L, 1.0f/2.4f) - 0.055f;
   }

   return S;
}

// Vector operations
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

union v4
{
    struct {
        union
        {
            v3 xyz;
            struct {
                f32 x, y, z;
            };
        };
        
        f32 w;        
    };
    struct {
        union
        {
            v3 rgb;
            struct {
                f32 r, g, b;
            };
        };
        
        f32 a;        
    };
    struct {
        v2 xy;
        f32 Ignored0_;
        f32 Ignored1_;
    };
    struct {
        f32 Ignored2_;
        v2 yz;
        f32 Ignored3_;
    };
    struct {
        f32 Ignored4_;
        f32 Ignored5_;
        v2 zw;
    };
    f32 E[4];
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
operator-(v3 A, v3 B)
{
    v3 Result;
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    return Result;
}

inline v3
operator+(v3 A, v3 B)
{
    v3 Result;
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    return Result;
}

inline v3 &
operator+=(v3 &A, v3 B)
{
    A = A + B;
    return A;
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

inline v3
Lerp(v3 A, v3 B, f32 t)
{
    v3 Result = A*(1.0f - t) + B*t;
    return Result;
}

inline v3
Hadamard(v3 A, v3 B)
{
    v3 Result;
    Result.x = A.x*B.x;
    Result.y = A.y*B.y;
    Result.z = A.z*B.z;
    return Result;
}

//
// v4 operations
//

inline u32
BGRAPack(v4 Color)
{
    u32 Result = ((RoundF32ToU32(Color.a) << 24) |
                  (RoundF32ToU32(Color.r) << 16) |
                  (RoundF32ToU32(Color.g) << 8)  |
                  (RoundF32ToU32(Color.b) << 0));

    return Result;
}

inline v4
V4(f32 X, f32 Y, f32 Z, f32 W)
{
    v4 Result;
    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    Result.w = W;
    return Result;
}

inline v4
V4(v3 XYZ, f32 W)
{
    v4 Result;
    Result.xyz = XYZ;
    Result.w = W;
    return Result;
}

