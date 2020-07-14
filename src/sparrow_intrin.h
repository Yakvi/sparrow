#if !defined(SPARROW_INTRIN_H)

// TODO(yakvi): CRT-free way of implementing:
// Sin, Cos, Exp, Log: http://gruntthepeon.free.fr/ssemath/
// Tan, ACos: https://hg.libsdl.org/SDL/file/5c894fec85b9/src/stdlib/SDL_stdlib.c ?
// ATan, ATan2 (also will need to test Sin/Cos implementations): https://github.com/michael-quinlan/ut-sse/blob/master/sse/sseMath.h
// TODO(yavki): __cpuid to check for SSE 4.1 usage!

inline f32
SquareRoot(f32 Value)
{
    f32 Result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(Value)));

    return (Result);
}

inline f32
Sin(f32 Value)
{
    f32 Result = 0;
    return (Result);
};

inline f32 Cos();
inline f32 Exponential();
inline f32 Logarithm();

inline f32 Tan();
inline f32 ACos();
inline f32 ATan();
inline f32 ATan2();

#define SPARROW_INTRIN_H
#endif