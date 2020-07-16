#if !defined(SPARROW_VECTOR_TYPES_H)

// BOOKMARK: Types
// NOTE: Floating point components

typedef union v2
{
    struct
    {
        float x, y;
    };

    struct
    {
        float u, v;
    };

    struct
    {
        float Left, Right;
    };

    struct
    {
        float Width, Height;
    };

    float Elements[2];

#ifdef __cplusplus
    inline float& operator[](const int& Index)
    {
        return Elements[Index];
    }
#endif //__cplusplus
} p, v2f, v2, dim;

typedef union v3
{
    struct
    {
        float x, y, z;
    };

    struct
    {
        float r, g, b;
    };

    struct
    {
        v2 xy;
        float Ignored0_;
    };

    struct
    {
        float Ignored1_;
        v2 yz;
    };

    float Elements[3];

#ifdef __cplusplus
    inline float& operator[](const int& Index)
    {
        return Elements[Index];
    }
#endif
} color3, rgb, v3f, v3;

typedef union v4
{
    struct
    {
        union
        {
            v3 xyz;
            struct
            {
                float x, y, z;
            };
        };

        float w;
    };
    struct
    {
        union
        {
            v3 rgb;
            struct
            {
                float r, g, b;
            };
        };

        float a;
    };

    struct
    {
        v2 xy;
        float Ignored0_;
        float Ignored1_;
    };

    struct
    {
        float Ignored2_;
        v2 yz;
        float Ignored3_;
    };

    struct
    {
        float Ignored4_;
        float Ignored5_;
        v2 zw;
    };

    __m128 VectorString;

    float Elements[4];

#ifdef __cplusplus
    inline float& operator[](const int& Index)
    {
        return Elements[Index];
    }
#endif
} color4, rgba, v4, v4f;

typedef union mat4
{
    float Elements[4][4];

    __m128 Columns[4];

#ifdef __cplusplus
    inline v4 operator[](const int& Index)
    {
        float* col = Elements[Index];

        v4 result;
        result.Elements[0] = col[0];
        result.Elements[1] = col[1];
        result.Elements[2] = col[2];
        result.Elements[3] = col[3];

        return result;
    }
#endif
} mat4;

typedef union quat
{
    struct
    {
        union
        {
            v3 xyz;
            struct
            {
                float x, y, z;
            };
        };

        float w;
    };

    float Elements[4];

    __m128 VectorString;
} quat;

// NOTE: Int components

typedef union v2i
{
    struct
    {
        s32 x;
        s32 y;
    };

    struct
    {
        s32 Width;
        s32 Height;
    };

    float Elements[2];

#ifdef __cplusplus
    inline float& operator[](const int& Index)
    {
        return Elements[Index];
    }
#endif //__cplusplus
} v2i, dim_2i;

#define SPARROW_VECTOR_TYPES_H
#endif
