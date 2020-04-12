#if !defined(WIN32_SPARROW_H)

struct win32_game_calls
{
    b32 IsLoaded;
    function Update;
    function Render;
};

#define WIN32_SPARROW_H
#endif
