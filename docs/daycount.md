# Project Sparrow - Day Count

## 1. Jan 16, 2020 - Hello World

## 2. Jan 25, 2020 - Hello Window

## 3. Jan 26, 2020 - Callbacks and Messages

## 4. Feb 03, 2020 - Hello Mouse

## 5. Feb 07, 2020 - Code refactoring and bug hunting

## 6. Feb 23, 2020 - Work on the primary output buffer

## 7. Feb 26, 2020 - Back to handmade hero

## 8. Feb 27, 2020 - Debugging fake Bitmap info

## 9. Feb 29, 2020 - Back to 2018!

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

You'll notice that instead of HDC and RECT types I'm using a void \* and my own rect type. That's an experiment that I've been continuing - rewriting only the most essential to me pieces of Windows API, and leaving everything else barebones.

I quickly realized that I wasn't getting a proper Device Context inside my WinMain. GetDC(HWND) would just return null. So my first assumption was that my custom Windows calls were busted. I tried to reimplement (several times) proper HDC struct:

    typedef struct {
        int unused;
    } _HDC;
    typedef _HDC HDC;

I also tried to replace a void\* with a simple u32 or u64. To no avail.

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

## 12. Apr 12, 2020 - Implementing DLL loading (+getting rid of CRT)

I set myself on the road of casey's Hot Reloading(tm) today. The idea was to try and reimplement the dll loading system without peaking or otherwise trying to cheat my way out of it.

Challenge: in the game's frame loop, we need to:

1. Check if we need to update the DLL.
2. If so, load DLL data
3. Get the necessary procedures
4. Copy them somewhere? Or otherwise ensure we don't need the dll file itself
5. Release the file handle
6. Inject the procedures in the game.

That way we can edit the dll source code and compile it on the fly, while the game is running.

Today, we have pretty much achieved 2, 3 and 6. We're loading the DLL file (with a relative path, this will need to be addressed eventually), getting its procedures and inject them into the game. To this end, I've introduced a new struct win32_game_calls and the Win32_TryLoadGameDLL call which also makes sure the game procedures have loaded succesfully. The resulting code is the following:

    typedef u64(__stdcall* function)();

    struct win32_game_calls
    {
        b32 IsLoaded;
        function Update;
        function Render;
    };

    local void
    Win32_TryLoadGameDLL(struct win32_game_calls* Game)
    {
        void* GameDLL = LoadLibraryA("sparrow.dll");
        if (GameDLL) {
            Game->Update = GetProcAddress(GameDLL, "UpdateState");
            Game->Render = GetProcAddress(GameDLL, "Render");
            Game->IsLoaded = (Game->Update && Game->Render); // Ensures both pointers are not 0.
        }
    }

Some time was also spent debugging why exactly my program wasn't binding with those functions. As it turns out, I forgot to include the -EXPORT:UpdateState and -EXPORT:Render linker commands in my build file.

Evidently, before we call this feature done we'll need to properly address the other points, as well as some minor todos I left along the way.

Another thing that caught my attention was the amount of garbage the exe and dll files were adding to their content. I'm not going to use any of the standard library (I suspect), so, as of now, it's of no use to me. What I have done as a result was introducing -NODEFAULTLIB linker flag, bringing down the size of my executable from 315 KiB to 6.5 KiB! I'm sure you can go even further than that but that's more than enough for the time being.

This also had a neat side-effect of having to define WinMainCRTStartup() which I'm sure I'll be able to use for my own means if I need it (or even get rid of WinMain entirely?).

## 13. Apr 17, 2020 - Continue hot reloading implementation

The past few days have consisted in a slow dance of opening the source files, poking a bit around, making refactorings here and there, reading some documentation, and closing it all. This resulted however in touching almost all of my codebase to the following chages:

- I started on a journey of incorporating my yak_defines.h inside the project. For now, I changed the includes to types.h which then redirects to yak_defines. The idea is that, once, I'm happy with the state of yak_defines, I'll simply copy the code from there into the types.h.
- sparrow.c: compressing \_DllMainCRTStartup even further. Not sure I can get rid of it entirely without linking CRT.
- win32_sparrow.c: continue work on live code reloading. My current approach is to rely on existance of the lock file, but a concrete solution still hasn't presented itself (and I'm not entirely happy with the one proposed by Casey). In the meantime, I've continued definition of the win32_module structure. I hope to find a solution to this in the next commit.

## 14. Apr 19, 2020 - Dynamic DLL Locking

Completed implementation of the lockfile-based DLL loading. The Win32_TryLoad will take any DLL and, provided the lockfile doesn't exist, load it. This functionality existed before. Now, however, the function will also unload any DLL is such a lock file already exists.

The cleanup code is super straightforward:

    if (Module->Library && FreeLibrary(Module->Library)) {
        Module->Library = 0;
        Module->Update = 0;
        Module->Render = 0;
    }

The big change however that allowed me to finally get to this piece of code was the introduction of string handling. The new sparrow_text.h defines the `text_buffer` struct and a couple helper functions.

Memory allocation happens using VirtualAlloc on Win32 layer and will use the memory arena in the dll layer, if needed. I'm currently using `memset` and `memcpy` which apparently are an integral part of the x64 MSVC++ compiler; will probably implement an intrinsic "library" later on for this kind of operations.

Overall list of the new symbols added:

`win32_sparrow.c`:

    local struct text_buffer Win32_AllocatePathBuffer(char* Input);
    local struct win32_module Win32_InitModule(struct text_buffer* ModuleDirectory, char* Filename);
    local struct text_buffer Win32_GetWorkingDirectory();
    local struct text_buffer Win32_GetModuleDirectory(char* WorkingDirectory);

`sparrow_text.h`:

    #define STR_SHORT 128
    #define STR_MEDIUM 512
    #define STR_MAX 4096
    struct text_buffer;

    inline unsigned int StringLength(char* String);
    inline void TextConcat(struct text_buffer* Buffer, char* NewText);
    inline struct text_buffer InitTextBuffer(char* Buffer, unsigned int Size, char* Input);

## 15. Apr 22, 2020 - Live Code Reloading and Debugging!

Today's change was largely away from the codebase, but rather a bit of tinkering within my build file. I'm trying to keep the build file as simple as possible, but for live code debugging I need to do some actions on build file side. At least, I didn't manage to find any acceptable analogue to the system implemented by Casey back in episodes 23-24.

Anyway, the idea is simple: each build, change the .pdb filename slightly so that it can be loaded by the debugger (debugger won't release the pdb file during the session, apparently). I'm adding mm-ss-ms time format at the end of the fixed pdb name, but it really can be any. I'm also trying to clean up by deleting any .pdb file I find in the build directory.

My implentation was achieving pretty much that, plus I optimized my code colouring macros. Still pretty sure there's an easier way for those.

Last thing, I started working on a platform layer todo list, now that I have some basic functionality up and running. This is far from perfect but at least this will allow me to continue making new stuff without kicking the old ideas through my mind.

## 16. Apr 25, 2020 - Implementing fixed-grid console

I started working on a console-like output. As of today, I can load a 120x40 grid into the DLL memory. Each pixel can be tinted in solid color, and contains information regarding its position on the grid and color.

![Console Output](media/Day16/hello_console2.png)

The system now has the following loops:

`GameUpdate`:

- `InitConsolePixels`: (Console Buffer) Resets all pixels position and color
- `PixelOverlay`: (Console Buffer) All pixels get a color overload.
  There's currently also a structured art iteration which allows to write some simple ASCII text art.

`Render`:

- `Clear`: (Render Buffer) Global pass through the whole buffer to clear it in one colour
- `DrawAllPixels`: (Console Buffer) Send all pixels for individual rendering
- `DrawPixel`: (RenderBuffer) Set specified memory location to the specified values.

Each "screen chunk" can potentially be expanded to carry more data (including bitmap, characters, etc.). I still have no idea how will I implement any decent text display, but if we're going all the way through console, might as well do it via Console Buffer. For now however, I implemented a simple string lookup tool to quickly lay out color.

![Console Output](media/Day16/hello_console.png)

I have no idea about how well this performs, so I will need to implement some frame-time tracking later on.

I also realized that, in release mode, intrinsic memset and memcopy are actually dropped, so I implemented the ones from [Handmade Network](https://hero.handmade.network/forums/code-discussion/t/94-guide_-_how_to_avoid_c_c++_runtime_on_windows/)

Up next: Create a decent random access scheme for the Console buffer.

## 17. April 30, 2020 - Fixing pixel grid orientation

The efforts to improve the console pixel grid continue. Today, the biggest implementation was that of the `GetPixel` function. We can now properly (and quickly?) load any pixel with the specified coordinates. The function itself its pretty simple, if you ignore all the assertions (that fired oh so many times during its implementation).

    local struct pixel*
    GetPixel(struct pixel* Pixels, v2u Coords)
    {
        u32 Y = CONSOLE_WIDTH * Coords.y;
        u32 Pos = Y + Coords.x;

        struct pixel* Result = Pixels + Pos;

        return (Result);
    }

Even this is a bit of an overkill but I wanted to be super explicit, should we need to revisit this function later down the line. For now, the plan is to use the new function to assist in implementing the first input system.

One big issue that was fixed was the coordinate system orientation of the pixel grid. Before today's work, the origin of the console grid was in the bottom-left corner of the buffer; now it's in the top-left. Hopefully this will allow easier iteration moving forward.

## 18. May 2, 2020 - Basic input processing!

`win32_sparrow.c`: Removed the the `ReadInput()` function, unified it with `Win32_ProcessMessages()`. The new function is called `Win32_ReadInput()` and it has the following area of competence:

- Read input provided by the Windows API
- If it's a keypress, handle it in the `Win32_NormalKeyInput()` routine

In the future, additional input processing functionality will be implemented here.

`sparrow_vector.h`: Implemented rounding functions for scalar and vectors, introduced `AddV2` function. Starting really to think about moving to cpp for those _sweet sweet_ function and operator overloaders.

`sparrow.c`: Defined `MovePlayer()` function. It adds the provided input vector to the player's position. It's then rounded out and the according cell is colorized. It's also does the screen wrapping if necessary.

![Basic input processing](media/Day18/day18.gif)

## 19. May 7, 2020 - String output

`win32_sparrow.c`: started first experiments with multithreading. Will return to it, eventually.
`sparrow.c`: Moved `PixelOverlay()` and `SetStructuredArt()` back.
`sparrow_console_update`: Introduced basic string parsing based on a character table. In theory, eventually we will be able parse any string thrown at us; at the moment though, we will crash if an unrecognized character. I did however introduce a whole bunch of glyphs (A-Z, a-z, 0-9 and a few special characters) for testing purposes. Should be enough for now, and this approach allows the glyph library to be expanded at the moment's notice. I'll probably need to identify a more compact storage option (u64 bits?).

As a result, we can now print simple strings on the grid using the following API:

`PrintString(*Pixels, *String, FirstCharacterPosition(top-left corner), Color)`

Another bug that I identified is inconsistent pixel size; this is especially visible with smaller pixel grids. This will need to be addressed in the future.

## 20. May 8, 2020 - Debugging pixel size

Today was a bit of an exercise in futility. In my rush to implement a rendering system I actually introudced a whole bunch of bugs that need to be addressed.

`win32_sparrow.c`: Removed fixed buffer size at initialization
`sparrow_console_render.c`: Identified an issue where sometimes a pixel is too big, and sometimes is too small. A proper stretching mechanism will need to be implemented if we are to solve this issue.

For the most part I have reverted my changes to the start of the day. I'll need to revisit console rendering code at a later stage.

## 21. May 15, 2020 - Revert DrawPixel changes

My attempts in removing scaling artifacts highlighted that I had a very vague understanding of the problem at hand. By now I mostly reverted the changes of the last few days except I streamlined the DrawPixel call a bit. I will need to revisit this part of code in the future; for now, as exploration code, it's more than enough.

## 22. May 27, 2020 - Improve text glyphs and start Everscroll

I cleaned up the text glyphs for the console. They are still solid pixels on a 8x8 grid, but at least there's some sort of kerning going on.

Additionally, I started work on a potential Everscroll module. The idea is that eventually it will be a third dll loaded from the main dll that will not know of core existance. However, I'm not sure this is still possible since all of these modules will have to live in the GameState structure. Maybe I can avoid it with some "clever" use of the void pointers.

## 23. June 04, 2020 - Mouse Cursor Capture

Implemented mouse cursor capture. In `win32_sparrow.c` we package both normalized and absolute mouse cursor position, while inside our Console application we use normalized cursor position to determine whether or not a pixel is currently under mouse cursor. Right now it's extremely semplicistic but, in the future, we might be able to move more advanced mouse detection deeper into the pixel logic.

## 24. June 09, 2020 - Basic UI functionality

We now have some basic UI API. We can draw Points, Lines (only vertical and horizontal for now), Boxes, Text boxes (size defined by the text within), Strings and finally Buttons! For now, the buttons don't detect mouse clicks but only hover, however there's already some Immediate Mode functionality built in. Hopefully it will continue improving as the time goes by.

One thing I'll need to focus on next would be cleaning up my calls. Things like `AddV2i((v2i){0, -20}, (v2i)CONSOLE_CENTER)` in the body of the function call are anything but readable, and in the long run it will hurt me. So changes must be made (transition to C++? Some style guide definitions?).

## 25. June 17, 2020 - Modules

Today, Core Update and Rendering was 'sort of' separated from the game logic. The latter is now running in its own Modules - smaller? dlls containing only the logic pertinent to game updates.

This presents a whole new world of challenges: how to convert my Console logic into a proper API? Right now I'm simply importing the whole functions, but maybe filling out some specific structures and passing them back to the engine would make more sense ("I don't care how you deal with the pixels, I just want this button/pixel there").

Additionally, I discovered that if I try transitioning into CPP, the API is TOTALLY borked. Again, something that will be probably solved once we define a proper set of APIs.

## 26. July 5, 2020 - Refactoring

A lot of refactoring happened today.

First, I finished implementing static linking for C++ modules. This was done by removing any C-specific code like inline struct definition. Where, for instance, I had `color Color = (color){0,0,0};` I now have `color Color = Color(0,0,0);`. This also meant implementing a bunch of helper functions and replacing the code. Fortunately, the codebase isn't that big for now, and it was definitely worth it.

My second refactoring focused on dynamic console allocation. I now removed all references to `CONSOLE_WIDTH` and `CONSOLE_HEIGHT` from code. Most of the drawing API now receives the pointer to the whole Console structure, if only to verify that the parameters requested are within allocated bounds.

This also means that now the modules potentially have direct control over the sizing of the console, and it can be modified at the runtime. For now, I defined parameters MAX_CONSOLE_WIDTH and MAX_CONSOLE_HEIGHT: this is a temp measure until we implement dynamic memory allocation from a bank/arena.

The need for arena is increasing. I'll get there, eventually.

## 27. July 12, 2020 - More Refactoring

I started research into "handmade" and otherwise CRT or STD-indipendent libraries for math.

One thing that I came up accross is the [HandmadeMath](https://github.com/HandmadeMath/Handmade-Math) library. It's wide, it's supposed to be fast, and it almost fills out all the boxes. Except it doesn't provide any trig functions out of the box (sinf, cosf, etc.). The library either uses `Math.h` (that we can't use since we moved away from all things CRT) or allows the users specify their own functions.

After some lengthy googling, I was surprised how few things are out there. These are largely described on one of the [last pages](http://gruntthepeon.free.fr/ssemath/) I came across:

    On the mac, you get the vsinf and friends (in the Accelerate framework) which are nice (there is a ppc version and an intel version, Apple rox) but closed-source, and restricted to macos..

    Both Intel and AMD have some sort of vector math library with SIMD sines and cosines, but

    * Intel MKL is not free (neither as beer, nor as speech)
    * AMD ACML is free, but no source is available. Morever the vector functions are only available in 64bits OSes !
    * Would you trust the intel MKL to run at full speed on AMD hardware ?

    Some time ago, I found out the Intel Approximate Math library. This one is completely free and open-source, and it provides SSE and SSE2 versions of many functions. But it has two drawbacks:

    * It is written as inline assembly, MASM style. The source is very targetted for MSVC/ICC so it is painful to use with gcc
    * As the name implies, it is approximated. And, well, I had no use for a sine which has garbage in the ten last bits.

So, I'm lucky that, once again, [the thread that started it all](https://hero.handmade.network/forums/code-discussion/t/94-guide_-_how_to_avoid_c_c++_runtime_on_windows) came to the rescue. Next time, I'll be able to invent most of the functions used by `handmade-math` and hopefully be able to cheese my way out of the remaining couple.

In the meantime, I started cleaning out my code in preparation of the dependencies introduction/adaptation. I moved around some vector/type code, removed header files cross-dependency, added back some standard headers for intrinsics and compiler-used code. As far as I can tell, this doesn't add any bloat to the executables. Speaking of, the release versions are currently sitting at the following values: 

    - Win32 executable                                  6656 bytes
    - Sparrow dll (with "console" rendering system)     4096 bytes
    - C modules (Writer, Everscroll)                    11.5 KiB each
    - C++ module (pretty barren at the moment)          3072 bytes

Noting it here for future reference.

## 28. July 14, 2020 - Implementing SSE Trigonometry

Most of today was spent implementing the SSE libraries I stumbled upon. While the [first few functions](http://gruntthepeon.free.fr/ssemath/) were ported without too much of a hassle, porting atan and atan2 from the [second library](https://github.com/michael-quinlan/ut-sse/blob/master/sse/sseMath.h) has proven to be quite a challenge. The whole library is written in a pretty obscure C++, with a lot of operator overloading, operation chaining, etc. It also doesn't help at all that I'm still quite timid around bitwise operations. Lastly, to say that my trigonometry isn't at its best is to lie profusely. I'm walking in the dark, trying to find any sort of light. Alas, DuckDuckGo wasn't too helpful this time around.

Nevertheless, I still brought 5 new math functions into the mix, both in `float` and `__m128`. So some progress was made, after all. And I had an additional "breakthrough" by the end of the day, by discovering [an extension](https://github.com/to-miz/sse_mathfun_extension/) to the [ssemath](http://gruntthepeon.free.fr/ssemath/) functions. If it follows the same pattern, implementing these should be a breeze. I'll be then missing only one function, `arccos`, to be able to implement [HandmadeMath](https://github.com/HandmadeMath/Handmade-Math), and even there, I believe I found [a faster workaround](https://hero.handmade.network/forums/code-discussion/t/986-quaternions) to avoid using it at all.

## 29. July 15, 2020 - Implementing more SSE Trigonometry

Most of today went into adapting code of [SSE Math Functions extensions](https://github.com/to-miz/sse_mathfun_extension/) to my tastes. It now works, in my opinion much better than my yesterday's attempts.

I'm done with Trig for now. I copied over [cephes](http://www.netlib.org/cephes/) math library and maybe even eventually will attempt to implement `asin` and `acos` (the latter, as it turns out, being a fancy inversion of `asin`). But I'm a bit fatigued by now. This is too much of an unfamiliar ground for me, so I'll try to avoid it. 

I spent the rest of the day reshuffling the file structure (again), as well as dependency structure (again). In the end, I settled for having a  separate `math` folder, and started redefining my vector types in the newly created `sparrow_vector_types.h`. Now the work will begin to bring [HandmadeMath](https://github.com/HandmadeMath/Handmade-Math) into my project.

## 30. July 16, 2020 - Writing vector helpers

I started building my vector library from a few scalar definitions (which might be eventually moved somewhere else) and vector initialization helpers. Nothing to write home about.

## 31. July 18, 2020 - Writing more vector helpers

I added Add, Subtract, Multiply and Divide functions to the different vector scenarios, as well as made extensions/shortcuts for C++. I then plugged it all in the viewer to test it out.

One thing that immediately jumped to my attention that I have swapped top and bottom. I mean, it kind of was a point beforehand, but I'm starting to think that I need to account for the origin position during the console definition. Will try to tackle it next time. 

Also yay, this project is officially one "month" old! Or about half a year, depends on your point of view. 