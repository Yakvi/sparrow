#if !defined(SPARROW_CONSOLE_PLATFORM_H)

#define CONSOLE_WIDTH 120
#define CONSOLE_HEIGHT 40
#define CONSOLE_SIZE ((CONSOLE_WIDTH) * (CONSOLE_HEIGHT))

struct pixel
{
    v2i Pos;
    color Color;
};

#define SPARROW_CONSOLE_PLATFORM_H
#endif
