// link with the precompiled sparrow_math_optimized.obj
#if !defined(SPARROW_MATH_OPTIMIZED_H)
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    float log_f(float Value);
    float exp_f(float Value);
    float sqrt_f(float Value);
    float sin_f(float AngleRad);
    float cos_f(float AngleRad);
    float atan_f(float AngleRad);
    // float atan2_f(float Y, float X);
    void sincos_f(float AngleRad, float* Sine, float* Cosine);

    __m128 log_ps(__m128 Value);
    __m128 exp_ps(__m128 Value);
    __m128 sin_ps(__m128 AngleRad);
    __m128 cos_ps(__m128 AngleRad);
    __m128 atan_ps(__m128 AngleRad);
    __m128 atan2_ps(__m128 Y, __m128 X);
    void sincos_ps(__m128 AngleRad, __m128* Sine, __m128* Cosine);

#ifdef __cplusplus
}
#endif // __cplusplus
#define SPARROW_MATH_OPTIMIZED_H
#endif // !SPARROW_MATH_OPTIMIZED_H
