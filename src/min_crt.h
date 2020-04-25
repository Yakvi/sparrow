#if !defined(MIN_CRT_H)

#ifdef __cplusplus
extern "C" int _fltused;
#else
int _fltused;
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#pragma function(memset)
    void* memset(void* dest, int c, size_t count)
    {
        char* bytes = (char*)dest;
        while (count--) {
            *bytes++ = (char)c;
        }
        return dest;
    }

#pragma function(memcpy)
    void* memcpy(void* dest, const void* src, size_t count)
    {
        char* dest8 = (char*)dest;
        const char* src8 = (const char*)src;
        while (count--) {
            *dest8++ = *src8++;
        }
        return dest;
    }
#ifdef __cplusplus
}
#endif

#define MIN_CRT_H
#endif
