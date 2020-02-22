#include "fake_windows.h"

local void
Win32_RegisterKeyInput(u64 KeyState)
{
    switch (KeyState) {

    case VK_ESCAPE: {
        PostQuitMessage(0);
    } break;

    default: {
    } break;
    }
}

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

local void
Win32_ProcessWindowMessages(struct user_input* Input)
{
    // TODO: Input should be processed via GetKeyState() stdcall,
    // instead of waiting for message to pop

    message Message;
    while (PeekMessageA(&Message, 0, 0, 0, true)) {
        switch (Message.message) {

        case WM_MOUSEMOVE:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: {
            v2 MousePos = {0};
            MousePos.x = (u32)Message.LParam & 0xffff;
            MousePos.y = (u32)(Message.LParam >> 16 & 0xffff);
            Win32_RegisterMouseInput(Input, MousePos, Message.WParam);
        } break;

        case WM_SYSKEYUP:
        case WM_KEYUP: {
            Win32_RegisterKeyInput(Message.WParam);
        } break;

        case WM_QUIT: {
            GlobalRunning = false;
        } break;

        default: {
            TranslateMessage(&Message);
            DispatchMessageA(&Message);
        } break;
        }
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
    Win32_ProcessWindowMessages(Input);
}