#if !defined(SPARROW_VECTOR_H)

#include "core/sparrow_types.h"
#include "core/sparrow_intrin.h"

//
// NOTE: Vector Initializations
//
EXTERN_C_START
// Vector2 with floating point elements
inline v2f
V2F(f32 X, f32 Y)
{
    v2f Result;

    Result.x = X;
    Result.y = Y;

    return (Result);
}

// Create a floating point vector2 from an integer pair
inline v2f
V2Fi(s32 X, s32 Y)
{
    v2f Result = V2F((f32)X, (f32)Y);
    return (Result);
}

inline dim
DIM_2F(f32 Width, f32 Height)
{
    dim Result = V2F(Width, Height);
    return (Result);
}

// Vector2 with integer elements
inline v2i
V2I(s32 X, s32 Y)
{
    v2i Result;

    Result.x = X;
    Result.y = Y;

    return (Result);
}

inline dim_2i
DIM_2I(s32 Width, s32 Height)
{
    dim_2i Result = V2I(Width, Height);
    return (Result);
}

// Create an integer vector2 from a floating point pair
inline v2i
V2If(f32 X, f32 Y)
{
    v2i Result = V2I((s32)X, (s32)Y); // TODO: floor?
    return (Result);
}

#ifdef __cplusplus
#endif // __cplusplus

// Vector 3
inline v3
V3F(f32 X, f32 Y, f32 Z)
{
    v3 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return (Result);
}

inline v3
V3(f32 X, f32 Y, f32 Z)
{
    v3 Result = V3F(X, Y, Z);
    return (Result);
}

// Create a floating point vector3 from integers
inline v3f
V3Fi(s32 X, s32 Y, s32 Z)
{
    v3f Result = V3F((f32)X, (f32)Y, (f32)Z);
    return (Result);
}

// Vector 4
inline v4f
V4F(f32 X, f32 Y, f32 Z, f32 W)
{
    v4f Result;
    Result.VectorString = _mm_setr_ps(X, Y, Z, W);
    return (Result);
}

inline v4
V4(f32 X, f32 Y, f32 Z, f32 W)
{
    v4 Result;
    Result.VectorString = _mm_setr_ps(X, Y, Z, W);
    return (Result);
}

// Create a floating point vector4 from integers
inline v4
V4Fi(s32 X, s32 Y, s32 Z, s32 W)
{
    v4 Result;
    Result.VectorString = _mm_setr_ps((f32)X, (f32)Y, (f32)Z, (f32)W);
    return (Result);
}
EXTERN_C_END

#ifdef __cplusplus
inline v2f
V2F(u32 X, u32 Y)
{
    // TODO: ensure that X and Y are lower than FLT_MAX
    return V2F((f32)X, (f32)Y);
}

inline v2f
V2F(s32 X, s32 Y)
{
    return V2Fi(X, Y);
}

inline v2i
V2I(u32 X, u32 Y)
{
    // TODO: ensure that unsigned integers are smaller than INT_MAX
    return V2I((s32)X, (s32)Y);
}

inline v2i
V2I(f32 X, f32 Y)
{
    return V2If(X, Y);
}

inline v3
V3(s32 X, s32 Y, s32 Z)
{
    return V3Fi(X, Y, Z);
}

inline v4
V4(s32 X, s32 Y, s32 Z, s32 W)
{
    v4 Result;
    Result.VectorString = _mm_setr_ps((f32)X, (f32)Y, (f32)Z, (f32)W);
    return (Result);
}
#endif // __cplusplus

//
// NOTE: Binary Vector Operations
//

EXTERN_C_START

// v2f
inline v2f
AddV2f(v2f A, v2f B)
{
    v2f Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return (Result);
}

inline v2f
SubV2f(v2f A, v2f B)
{
    v2f Result;
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return (Result);
}

inline v2f
MulV2f(v2f A, v2f B)
{
    v2f Result;
    Result.x = A.x * B.x;
    Result.y = A.y * B.y;

    return (Result);
}

inline v2f
MulV2fReal(v2f A, f32 B)
{
    v2f Result;
    Result.x = A.x * B;
    Result.y = A.y * B;

    return (Result);
}

inline v2f
DivV2f(v2f A, v2f B)
{
    Assert(B.x);
    Assert(B.y);

    v2f Result;
    Result.x = A.x / B.x;
    Result.y = A.y / B.y;

    return (Result);
}

inline v2f
DivV2fReal(v2f A, f32 B)
{
    Assert(B);

    v2f Result;
    Result.x = A.x / B;
    Result.y = A.y / B;

    return (Result);
}

inline b32
CompareV2f(v2f A, v2f B)
{
    b32 Result = (A.x == B.x) && (A.y == B.y);

    return (Result);
}

// v2i
inline v2i
RoundV2ToV2i(v2f A)
{
    v2i Result;
    Result.x = RoundF32ToInt(A.x);
    Result.y = RoundF32ToInt(A.y);
    return (Result);
}

inline v2i
AddV2i(v2i A, v2i B)
{
    v2i Result;
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return (Result);
}

inline v2i
SubV2i(v2i A, v2i B)
{
    v2i Result;
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return (Result);
}

inline v2i
MulV2i(v2i A, v2i B)
{
    v2i Result;
    Result.x = A.x * B.x;
    Result.y = A.y * B.y;

    return (Result);
}

inline v2i
MulV2iReal(v2i A, f32 B)
{
    v2i Result;
    Result.x = (s32)(A.x * B);
    Result.y = (s32)(A.y * B);

    return (Result);
}

inline v2i
DivV2i(v2i A, v2i B)
{
    Assert(B.x);
    Assert(B.y);

    v2i Result;
    Result.x = A.x / B.x;
    Result.y = A.y / B.y;

    return (Result);
}

inline v2i
DivV2iReal(v2i A, f32 B)
{
    Assert(B);

    v2i Result;
    Result.x = (s32)(A.x / B);
    Result.y = (s32)(A.y / B);

    return (Result);
}

inline b32
CompareV2i(v2i A, v2i B)
{
    b32 Result = (A.x == B.x) && (A.y == B.y);

    return (Result);
}

// v3
inline v3
AddV3(v3 A, v3 B)
{
    v3 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;

    return (Result);
}

inline v3
SubV3(v3 A, v3 B)
{
    v3 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;

    return (Result);
}

inline v3
MulV3(v3 A, v3 B)
{
    v3 Result;

    Result.x = A.x * B.x;
    Result.y = A.y * B.y;
    Result.z = A.z * B.z;

    return (Result);
}

inline v3
MulV3Real(v3 A, f32 B)
{
    v3 Result;

    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;

    return (Result);
}

inline v3
DivV3(v3 A, v3 B)
{
    Assert(B.x);
    Assert(B.y);
    Assert(B.z);

    v3 Result;

    Result.x = A.x / B.x;
    Result.y = A.y / B.y;
    Result.z = A.z / B.z;

    return (Result);
}

inline v3
DivV3Real(v3 A, f32 B)
{
    Assert(B);

    v3 Result;

    Result.x = A.x / B;
    Result.y = A.y / B;
    Result.z = A.z / B;

    return (Result);
}

inline b32
CompareV3(v3 A, v3 B)
{
    b32 Result = (A.x == B.x) && (A.y == B.y) && (A.z == B.z);

    return (Result);
}

// v4
inline v4
AddV4(v4 A, v4 B)
{
    v4 Result;
    Result.VectorString = _mm_add_ps(A.VectorString, B.VectorString);

    return (Result);
}

inline v4
SubV4(v4 A, v4 B)
{
    v4 Result;
    Result.VectorString = _mm_sub_ps(A.VectorString, B.VectorString);

    return (Result);
}

inline v4
MulV4(v4 A, v4 B)
{
    v4 Result;
    Result.VectorString = _mm_mul_ps(A.VectorString, B.VectorString);

    return (Result);
}

inline v4
MulV4Real(v4 A, f32 B)
{
    v4 Result;
    __m128 BWide = _mm_set1_ps(B);
    Result.VectorString = _mm_mul_ps(A.VectorString, BWide);

    return (Result);
}

inline v4
DivV4(v4 A, v4 B)
{
    Assert(B.x);
    Assert(B.y);
    Assert(B.z);
    Assert(B.w);

    v4 Result;
    Result.VectorString = _mm_div_ps(A.VectorString, B.VectorString);

    return (Result);
}

inline v4
DivV4Real(v4 A, f32 B)
{
    Assert(B);

    v4 Result;
    __m128 BWide = _mm_set1_ps(B);
    Result.VectorString = _mm_div_ps(A.VectorString, BWide);

    return (Result);
}

inline b32
CompareV4(v4 A, v4 B)
{
    b32 Result = (A.x == B.x) && (A.y == B.y) && (A.z == B.z) && (A.w == B.w);

    return (Result);
}

EXTERN_C_END

#ifdef __cplusplus
// clang-format off
inline v2f Add(v2f A, v2f B) { return AddV2f(A, B); }
inline v2i Add(v2i A, v2i B) { return AddV2i(A, B); }
inline v3  Add(v3 A, v3 B)   { return AddV3(A, B);  }
inline v4  Add(v4 A, v4 B)   { return AddV4(A, B);  }

inline v2f Sub(v2f A, v2f B) { return SubV2f(A, B); }
inline v2i Sub(v2i A, v2i B) { return SubV2i(A, B); }
inline v3  Sub(v3 A, v3 B)   { return SubV3(A, B);  }
inline v4  Sub(v4 A, v4 B)   { return SubV4(A, B);  }

inline v2f Mul(v2f A, v2f B) { return MulV2f(A, B);     }
inline v2f Mul(v2f A, f32 B) { return MulV2fReal(A, B); }
inline v2i Mul(v2i A, v2i B) { return MulV2i(A, B);     }
inline v2i Mul(v2i A, f32 B) { return MulV2iReal(A, B); }
inline v3  Mul(v3 A, v3 B)   { return MulV3(A, B);      }
inline v3  Mul(v3 A, f32 B)  { return MulV3Real(A, B);  }
inline v4  Mul(v4 A, v4 B)   { return MulV4(A, B);      }
inline v4  Mul(v4 A, f32 B)  { return MulV4Real(A, B);  }

inline v2f Div(v2f A, v2f B) { return DivV2f(A, B);     }
inline v2f Div(v2f A, f32 B) { return DivV2fReal(A, B); }
inline v2i Div(v2i A, v2i B) { return DivV2i(A, B);     }
inline v2i Div(v2i A, f32 B) { return DivV2iReal(A, B); }
inline v3  Div(v3 A, v3 B)   { return DivV3(A, B);      }
inline v3  Div(v3 A, f32 B)  { return DivV3Real(A, B);  }
inline v4  Div(v4 A, v4 B)   { return DivV4(A, B);      }
inline v4  Div(v4 A, f32 B)  { return DivV4Real(A, B);  }

inline b32 Compare(v2f A, v2f B) { return CompareV2f(A, B); }
inline b32 Compare(v2i A, v2i B) { return CompareV2i(A, B); }
inline b32 Compare(v3 A, v3 B)   { return CompareV3(A, B);  }
inline b32 Compare(v4 A, v4 B)   { return CompareV4(A, B);  }

// operators

inline v2f operator+(v2f A, v2f B) { return AddV2f(A, B); }
inline v2i operator+(v2i A, v2i B) { return AddV2i(A, B); }
inline v3  operator+(v3 A, v3 B)   { return AddV3(A, B);  }
inline v4  operator+(v4 A, v4 B)   { return AddV4(A, B);  }

inline v2f operator-(v2f A, v2f B) { return SubV2f(A, B); }
inline v2i operator-(v2i A, v2i B) { return SubV2i(A, B); }
inline v3  operator-(v3 A, v3 B)   { return SubV3(A, B);  }
inline v4  operator-(v4 A, v4 B)   { return SubV4(A, B);  }

inline v2f operator*(v2f A, v2f B) { return MulV2f(A, B);     }
inline v2f operator*(v2f A, f32 B) { return MulV2fReal(A, B); }
inline v2i operator*(v2i A, v2i B) { return MulV2i(A, B);     }
inline v2i operator*(v2i A, f32 B) { return MulV2iReal(A, B); }
inline v3  operator*(v3 A, v3 B)   { return MulV3(A, B);      }
inline v3  operator*(v3 A, f32 B)  { return MulV3Real(A, B);  }
inline v4  operator*(v4 A, v4 B)   { return MulV4(A, B);      }
inline v4  operator*(v4 A, f32 B)  { return MulV4Real(A, B);  }

inline v2f operator/(v2f A, v2f B) { return DivV2f(A, B);     }
inline v2f operator/(v2f A, f32 B) { return DivV2fReal(A, B); }
inline v2i operator/(v2i A, v2i B) { return DivV2i(A, B);     }
inline v2i operator/(v2i A, f32 B) { return DivV2iReal(A, B); }
inline v3  operator/(v3 A, v3 B)   { return DivV3(A, B);      }
inline v3  operator/(v3 A, f32 B)  { return DivV3Real(A, B);  }
inline v4  operator/(v4 A, v4 B)   { return DivV4(A, B);      }
inline v4  operator/(v4 A, f32 B)  { return DivV4Real(A, B);  }

inline b32 operator==(v2f A, v2f B) { return CompareV2f(A, B); }
inline b32 operator==(v2i A, v2i B) { return CompareV2i(A, B); }
inline b32 operator==(v3 A, v3 B)   { return CompareV3(A, B);  }
inline b32 operator==(v4 A, v4 B)   { return CompareV4(A, B);  }

inline b32 operator!=(v2f A, v2f B) { return !CompareV2f(A, B); }
inline b32 operator!=(v2i A, v2i B) { return !CompareV2i(A, B); }
inline b32 operator!=(v3 A, v3 B)   { return !CompareV3(A, B);  }
inline b32 operator!=(v4 A, v4 B)   { return !CompareV4(A, B);  }

inline v2f& operator+=(v2f& A, v2f B) { return (A = A + B); }
inline v2i& operator+=(v2i& A, v2i B) { return (A = A + B); }
inline v3&  operator+=(v3& A, v3 B)   { return (A = A + B); }
inline v4&  operator+=(v4& A, v4 B)   { return (A = A + B); }

inline v2f& operator-=(v2f& A, v2f B) { return (A = A - B); }
inline v2i& operator-=(v2i& A, v2i B) { return (A = A - B); }
inline v3&  operator-=(v3& A, v3 B)   { return (A = A - B); }
inline v4&  operator-=(v4& A, v4 B)   { return (A = A - B); }

inline v2f& operator*=(v2f& A, v2f B) { return (A = A * B); }
inline v2f& operator*=(v2f& A, f32 B) { return (A = A * B); }
inline v2i& operator*=(v2i& A, v2i B) { return (A = A * B); }
inline v2i& operator*=(v2i& A, f32 B) { return (A = A * B); }
inline v3&  operator*=(v3& A, v3 B)   { return (A = A * B); }
inline v3&  operator*=(v3& A, f32 B)  { return (A = A * B); }
inline v4&  operator*=(v4& A, v4 B)   { return (A = A * B); }
inline v4&  operator*=(v4& A, f32 B)  { return (A = A * B); }

inline v2f& operator/=(v2f& A, v2f B) { return (A = A / B); }
inline v2f& operator/=(v2f& A, f32 B) { return (A = A / B); }
inline v2i& operator/=(v2i& A, v2i B) { return (A = A / B); }
inline v2i& operator/=(v2i& A, f32 B) { return (A = A / B); }
inline v3&  operator/=(v3& A, v3 B)   { return (A = A / B); }
inline v3&  operator/=(v3& A, f32 B)  { return (A = A / B); }
inline v4&  operator/=(v4& A, v4 B)   { return (A = A / B); }
inline v4&  operator/=(v4& A, f32 B)  { return (A = A / B); }

// clang-format on
#endif // __cplusplus

//
// NOTE: Vector operations
//

// TODO: length, length_squared, dot, cross, unit vector(normalize)

#define SPARROW_VECTOR_H
#endif
