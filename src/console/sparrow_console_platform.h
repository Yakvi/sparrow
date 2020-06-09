#if !defined(SPARROW_CONSOLE_PLATFORM_H)

#define CONSOLE_WIDTH 400  // 140
#define CONSOLE_HEIGHT 200 // 40
#define CONSOLE_SIZE ((CONSOLE_WIDTH) * (CONSOLE_HEIGHT))
#define CONSOLE_CENTER                                            \
    {                                                             \
        (int)(CONSOLE_WIDTH * 0.5f), (int)(CONSOLE_HEIGHT * 0.5f) \
    }

enum PixelFlags
{
    Pixel_Hovered = 0x1,
    Pixel_Clicked = 0x2,
    Pixel_Selected = 0x4
};

struct pixel
{
    v2i Pos;
    color Color;
    u32 Flags;
};

struct console
{
    struct pixel Pixels[CONSOLE_WIDTH * CONSOLE_HEIGHT]; // TODO(yakvi): Abstract it out? Overkill?
    dim PixelDim;
    b32 IsInitialized;
    v2i CursorPos;
};

local void InitConsole(struct console* Console, color Color);
local struct pixel* GetPixel(struct pixel* Pixels, v2i Coords);
local void PrintGlyph(struct pixel* Pixels, char* Char, v2i Pos, v3 Color);
local void PrintString(struct pixel* Pixels, char* String, v2i Pos, v3 Color);
local void VerticalGradient(struct pixel* Pixels, color Start, color End);
inline void Point(struct pixel* Pixels, v2i Pos, v3 Color);
local void Line(struct pixel* Pixels, v2i Left, u32 Length, u32 Direction, color Color);
local void Box(struct pixel* Pixels, v2i TopLeft, dim_2d Dim, color BoxColor);
local dim_2d TextBox(struct pixel* Pixels, v2i TopLeft, color BoxColor, char* Input, color StringColor);
local b32 Button(struct pixel* Pixels, v2i CursorPos, v2i TopLeft, char* Input);

inline void
V2iGridClamp(v2i* Pos)
{
    if (Pos->x >= CONSOLE_WIDTH) {
        Pos->x = CONSOLE_WIDTH - 1;
    }
    else if (Pos->x < 0) {
        Pos->x = 0;
    }
    if (Pos->y >= CONSOLE_HEIGHT) {
        Pos->y = CONSOLE_HEIGHT - 1;
    }
    else if (Pos->y < 0) {
        Pos->y = 0;
    }
}
inline void
V2fGridClamp(v2f* Pos)
{
    if (Pos->x >= CONSOLE_WIDTH) {
        Pos->x = 0;
    }
    else if (Pos->x < 0) {
        Pos->x = CONSOLE_WIDTH - 1;
    }
    if (Pos->y >= CONSOLE_HEIGHT) {
        Pos->y = 0;
    }
    else if (Pos->y < 0) {
        Pos->y = CONSOLE_HEIGHT - 1;
    }
}

#define SPARROW_CONSOLE_PLATFORM_H
#endif
