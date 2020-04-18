#include "sparrow.h"

local struct game_state*
LoadGameState(struct memory* Memory)
{

    struct game_state* Result = 0;
    if (Memory) {
        Result = (struct game_state*)Memory->Data;
        if (!Result->IsInitialized) {
            Result->GradientXOffset = 0;
            Result->GradientYOffset = 0;
            Result->IsInitialized = true;
        }
    }

    return (Result);
}

void
UpdateState(struct memory* Memory,
            struct user_input* Input)
{
    struct game_state* GameState = LoadGameState(Memory);
    GameState->GradientXOffset++;
    GameState->GradientYOffset++;
}

void
Render(struct memory* Memory, struct frame_buffer* Buffer)
{
    struct game_state* GameState = LoadGameState(Memory);
    u32 xOffset = GameState ? GameState->GradientXOffset : 0;
    u32 yOffset = GameState ? GameState->GradientYOffset : 0;

    if (Buffer) {
        u32 Pitch = Buffer->Width * Buffer->BytesPerPixel;
        u8* Row = Buffer->Pixels;
        for (u32 y = 0;
             y < Buffer->Height;
             ++y) {
            u32* Pixel = (u32*)Row;
            for (u32 x = 0;
                 x < Buffer->Width;
                 ++x) {
                u8 Red = (u8)(y - xOffset);
                u8 Green = (u8)(x + y);
                u8 Blue = (u8)(x + yOffset);

                *Pixel++ = ((Red << 16) | (Green << 8) | Blue | 0);
            }
            Row += Pitch;
        }
    }
}

#if _WIN32
// NOTE(yakvi): Required entry point for all win32 dlls
b32 __stdcall _DllMainCRTStartup()
{
    return true;
}
#endif