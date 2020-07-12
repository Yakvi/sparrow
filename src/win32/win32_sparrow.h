#if !defined(WIN32_SPARROW_H)

#include "min_crt.h"
#include "sparrow_types.h"
#include "sparrow_math.h"
#include "sparrow_platform.h"
#include "sparrow_text.h"
#include "fake_windows.h"

struct win32_module
{
    char* Name;
    void* Library;
    function Main;
    function Render;

    b32 IsLoaded;
};

#define WIN32_SPARROW_H
#endif
