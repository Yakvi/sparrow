#if !defined(WIN32_SPARROW_H)

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
