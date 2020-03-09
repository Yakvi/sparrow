# Project Sparrow - Day Count

## 1.  Jan 16, 2020 - Hello World
## 2.  Jan 25, 2020 - Hello Window
## 3.  Jan 26, 2020 - Callbacks and Messages
## 4.  Feb 03, 2020 - Hello Mouse
## 5.  Feb 07, 2020 - Code refactoring and bug hunting
## 6.  Feb 23, 2020 - Work on the primary output buffer
## 7.  Feb 26, 2020 - Back to handmade hero
## 8.  Feb 27, 2020 - Debugging fake Bitmap info 
## 9.  Feb 29, 2020 - Back to 2018! 
## 10. Mar 08, 2020 - Fighting Windows API
## 11. Mar 09, 2020 - Hunting for pointer loss bugs

After I tried to add some animation to my main buffer, I realized that my WinMain wasn't updating the animation. Moreover, if I removed the render loop from WM_PAINT, my render buffer would fail to render at all! 

At that time, my code looked the following way (simplifying and stripping it down): 

    MainMemory = Win32_MainMemoryInit(KiB(10)); // stored as global
    while (GlobalRunning){
        [...]
        UpdateState(MainMemory);
        Render(MainMemory)

        void* DeviceContext = GetDC(Window);
        rect WindowRect;
        GetClientRect(Window, &WindowRect);
        Win32_UpdateBuffer(DeviceContext, WindowRect);
        ReleaseDC(Window, DeviceContext);
    }

You'll notice that instead of HDC and RECT types I'm using a void * and my own rect type. That's an experiment that I've been continuing - rewriting only the most essential to me pieces of Windows API, and leaving everything else barebones. 

I quickly realized that I wasn't getting a proper Device Context inside my WinMain. GetDC(HWND) would just return null. So my first assumption was that my custom Windows calls were busted. I tried to reimplement (several times) proper HDC struct:

    typedef struct {
        int unused;
    } _HDC;
    typedef _HDC HDC;

I also tried to replace a void* with a simple u32 or u64. To no avail.

Additional debugging showed that my Window handle would also become null! Moreover, after entering the loop above for the first time, it would quickly go from its normal pointer value to something like 0x0000000100010001, before eventually zeroing out completely. Something similar was happening to my DeviceContext, as well. 

My searches led me to understand that something was happening inside the UpdateState() and Render() functions which were both getting MainMemory struct. Finally, isolating some code and by slowly releasing it I was able to find the culprit: the MainMemory allocation block!

The memory struct, as of now, is super simple: 

    struct memory
    {
        memory_index Size;
        void* Data;
    };

And the code to initialize it was simple as well: 

    local struct memory*
    Win32_MainMemoryInit(memory_index Size)
    {
        struct memory* Result = 
            Win32_MemoryAlloc(Size + sizeof(struct memory));
            // Fancy wrapper to call VirtualAlloc
        Result->Size = Size;
        Result->Data = &Result + sizeof(struct memory);

        return (Result);
    }

And it's in the function above that a trained eye will immediately spot the bug: I was initializing Data to point not at the virtual space provided to me by Windows, but directly on the stack, a few bytes after the pointer itself! And, of course, back at the main loop that data was occupied by the Windows and, later, DC. 

Removing the small & symbol was all it took! So much for hours of debugging spread accross two days. 