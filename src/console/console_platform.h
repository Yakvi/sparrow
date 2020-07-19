#if !defined(SPARROW_CONSOLE_PLATFORM_H)

#define MODULE_DEF(memory) (memory, struct user_input * Input, struct console * Console)
#define MODULE_MAIN(memory) void ModuleMain MODULE_DEF(memory)
typedef void(*console_module) MODULE_DEF(void* ModuleMemory);

#define MAX_CONSOLE_WIDTH 800  // 140
#define MAX_CONSOLE_HEIGHT 800 // 40

enum PixelFlags
{
    Pixel_Hovered  = 0x1,
    Pixel_Clicked  = 0x2,
    Pixel_Selected = 0x4
};

struct pixel
{
    v2i    Pos;
    color3 Color;
    u32    Flags;
};

enum ConsolePixelOrder
{
    Console_TopDown,
    Console_BottomUp
};

struct console
{
    dim PixelDim;
    b32 IsInitialized;
    v2i CursorPos;

    dim_2i Size;
    dim    InvertedSize;

    u32 PixelCount;
    v2i Center;

    u32 PixelOrder;

    struct pixel Pixels[MAX_CONSOLE_HEIGHT * MAX_CONSOLE_WIDTH]; // TODO(yakvi): Abstract it out? Overkill?
};

local void          InitConsole(struct console* Console, s32 Width, s32 Height, u32 PixelOrder, color3 Color);
local struct pixel* GetPixel(struct console* Console, v2i Coords);
local void          PrintGlyph(struct console* Console, char* Char, v2i Pos, color3 Color);
local void          PrintString(struct console* Console, char* String, v2i Pos, color3 Color);
local void          VerticalGradient(struct console* Console, color3 Start, color3 End);
inline void         Point(struct console* Console, v2i Pos, v3 Color);
local void          Line(struct console* Console, v2i Left, u32 Length, u32 Direction, color3 Color);
local void          Box(struct console* Console, v2i TopLeft, dim_2i Dim, color3 BoxColor);
local dim_2i        TextBox(struct console* Console, v2i TopLeft, color3 BoxColor, char* Input, color3 StringColor);
local b32           ButtonHover(struct console* Console, v2i CursorPos, v2i TopLeft, char* Input);

inline void
V2iGridClamp(struct console* Console, v2i* Pos)
{
    if (Pos->x >= Console->Size.Width) {
        Pos->x = Console->Size.Width - 1;
    }
    else if (Pos->x < 0) {
        Pos->x = 0;
    }
    if (Pos->y >= Console->Size.Height) {
        Pos->y = Console->Size.Height - 1;
    }
    else if (Pos->y < 0) {
        Pos->y = 0;
    }
}
inline void
V2fGridClamp(struct console* Console, v2f* Pos)
{
    if (Pos->x >= Console->Size.Width) {
        Pos->x = 0;
    }
    else if (Pos->x < 0) {
        Pos->x = (f32)Console->Size.Width - 1;
    }
    if (Pos->y >= Console->Size.Height) {
        Pos->y = 0;
    }
    else if (Pos->y < 0) {
        Pos->y = (f32)Console->Size.Height - 1;
    }
}

#define SPARROW_CONSOLE_PLATFORM_H
#endif
