#include "fake_windows.h"

local void
Win32_RegisterMouseInput(struct user_input* Input, v2 MousePos, u64 MouseState)
{
    if (Input->LeftMouseClick && !(MouseState & MK_LBUTTON)) {
        Input->LeftMouseClick = false;
        OutputDebugStringA("Mouse released\n");
    }
    switch (MouseState) {

        case MK_LBUTTON: {
            if (!Input->LeftMouseClick) {
                OutputDebugStringA("Mouse pressed\n");
                Input->LeftMouseClick = true;
            }
            // TODO: count half clicks?
        } break;

        default: {
        } break;
    }
}

local struct user_input*
InitializeInput(void)
{
    struct user_input* Result = Win32_MemoryAlloc(sizeof(struct user_input));
    return (Result);
}

local void
ReadInput(struct user_input* Input)
{
    // BOOKMARK: Register mouse input if any
}