#if !defined(WIN32_SPARROW_H)

#include "min_crt.h"
#include "types.h"
#include "fake_windows.h"
#include "sparrow_text.h"
#include "sparrow_platform.h"

struct win32_module
{
    char* Name;
    void* Library;
    function Update;
    function Render;

    b32 IsLoaded;
};

#define WIN32_SPARROW_H
#endif
