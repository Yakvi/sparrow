#include "fake_windows.h"

local struct user_input*
InitializeInput(void)
{
    struct user_input* Result = Win32_MemoryAlloc(sizeof(struct user_input));
    return (Result);
}

local void
Win32_ClearInput(struct user_input* Input)
{
    Input->LeftMouseClick = false;
    Input->MovementKeys = (v2){0, 0};
}

local void
Win32_RegisterMouseInput(struct user_input* Input, v2 MousePos, u64 MouseState)
{
    // TODO THIS IS A STUB
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
Win32_NormalKeyInput(struct user_input* Input, u64 Key)
{
    switch (Key) {

        case VK_ESCAPE: {
            GlobalRunning = false;
        } break;

        case 'D':
        case VK_RIGHT: {
            Input->MovementKeys.x += 1;
        } break;

        case 'A':
        case VK_LEFT: {
            Input->MovementKeys.x -= 1;
        } break;

        case 'S':
        case VK_DOWN: {
            Input->MovementKeys.y += 1;
        } break;

        case 'W':
        case VK_UP: {
            Input->MovementKeys.y -= 1;
        } break;

        default: {
        } break;
    }
}

local void
Win32_ReadInput(struct user_input* Input)
{
    Win32_ClearInput(Input); // TODO: Backup old input before clearing?
    message Message;
    while (PeekMessageA(&Message, 0, 0, 0, true)) {
        switch (Message.message) {

            case WM_MOUSEMOVE:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP: {
                // TODO: Do we even want to handle any mouse code here?
            } break;

            case WM_SYSKEYUP:
            case WM_KEYDOWN: {
                // TODO: Different handling of keys if system keys are pressed (ALT/CTRl/SHIFT)
                Win32_NormalKeyInput(Input, Message.WParam);
            } break;

            default: {
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            } break;
        }
    }
    // TODO: Register mouse input if any
}