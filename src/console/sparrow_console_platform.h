#if !defined(SPARROW_CONSOLE_PLATFORM_H)

#define CONSOLE_WIDTH 350
#define CONSOLE_HEIGHT 200
#define CONSOLE_SIZE ((CONSOLE_WIDTH) * (CONSOLE_HEIGHT))

struct pixel
{
    v2i Pos;
    color Color;
};

#define SPARROW_CONSOLE_PLATFORM_H
#endif