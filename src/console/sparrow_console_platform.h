#if !defined(SPARROW_CONSOLE_PLATFORM_H)

#define CONSOLE_WIDTH 140
#define CONSOLE_HEIGHT 40
#define CONSOLE_SIZE ((CONSOLE_WIDTH) * (CONSOLE_HEIGHT))
#define CONSOLE_CENTER                                            \
    {                                                             \
        (int)(CONSOLE_WIDTH * 0.5f), (int)(CONSOLE_HEIGHT * 0.5f) \
    }

struct pixel
{
    v2i Pos;
    color Color;
};

struct console
{
    struct pixel Pixels[CONSOLE_WIDTH * CONSOLE_HEIGHT]; // TODO(yakvi): Abstract it out? Overkill?
    dim PixelDim;
};

#define SPARROW_CONSOLE_PLATFORM_H
#endif
