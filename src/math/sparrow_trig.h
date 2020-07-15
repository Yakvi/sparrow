// link with the precompiled sparrow_trig_opt.obj
#if !defined(SPARROW_TRIG_H)
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    // NOTE: For performance-critical path go for wider analogue

    float log_f(float Value);
    float exp_f(float Value);
    // _mm_sqrt_ps, square root of value
    float sqrt_f(float Value);
    // _mm_sqrt_ps, 1 / square root of value
    float rsqrt_f(float Value);
    float sin_f(float AngleRad);
    float cos_f(float AngleRad);
    float tan_f(float AngleRad);
    float cot_f(float AngleRad);
    float atan_f(float AngleRad);
    float atan2_f(float Y, float X);
    void sincos_f(float AngleRad, float* Sine, float* Cosine);

    __m128 log_ps(__m128 Value);
    __m128 exp_ps(__m128 Value);
    // sqrt_ps: call _mm_sqrt_ps directly
    // rsqrt_ps: call _mm_rsqrt_ps directly

    __m128 sin_ps(__m128 AngleRad);
    __m128 cos_ps(__m128 AngleRad);
    __m128 tan_ps(__m128 AngleRad);
    __m128 cot_ps(__m128 AngleRad);
    __m128 atan_ps(__m128 AngleRad);
    __m128 atan2_ps(__m128 Y, __m128 X);
    void sincos_ps(__m128 AngleRad, __m128* Sine, __m128* Cosine);

#ifdef __cplusplus
}
#endif // __cplusplus
#define SPARROW_TRIG_H
#endif // !SPARROW_TRIG_H
