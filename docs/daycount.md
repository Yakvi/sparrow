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

I spent the rest of the day reshuffling the file structure (again), as well as dependency structure (again). In the end, I settled for having a separate `math` folder, and started redefining my vector types in the newly created `sparrow_vector_types.h`. Now the work will begin to bring [HandmadeMath](https://github.com/HandmadeMath/Handmade-Math) into my project.

## 30. July 16, 2020 - Writing vector helpers

I started building my vector library from a few scalar definitions (which might be eventually moved somewhere else) and vector initialization helpers. Nothing to write home about.

## 31. July 18, 2020 - Writing more vector helpers

I added Add, Subtract, Multiply and Divide functions to the different vector scenarios, as well as made extensions/shortcuts for C++. I then plugged it all in the viewer to test it out.

One thing that immediately jumped to my attention that I have swapped top and bottom. I mean, it kind of was a point beforehand, but I'm starting to think that I need to account for the origin position during the console definition. Will try to tackle it next time.

Also yay, this project is officially one "month" old! Or about half a year, depends on your point of view.

## 31. July 19, 2020 - Raytracing basics

I finally started making my way through the [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) ebook-tutorial. It was actually this project which prompted me to:

- Introduce a module system
- Implement C++ support in the core modules
- Build some trig functions (courtesy of [Julien Pommier](http://gruntthepeon.free.fr/ssemath/) and [Tolga Mizrak](https://github.com/to-miz/sse_mathfun_extension))
- Build a pretty comprehensive vector library (courtesy of [Zakary Strange et al.](https://github.com/HandmadeMath/Handmade-Math/))

So, basically, day 25 onwards. Not bad at all, all things considering. Now, looking back at the changes of today.

First of all, I finalized the `sparrow_vector.h`, for now. Many interesting things remain in the `HandmadeMath`, like matrix operations, common graphics actions, etc. But, for what's it worth, for now I feel that's enough. Already we're talking about dozens of symbols, between the pure C implementations, shortcut functions, as well as C++ overrides and operator functions. This stuff creeps up on you pretty fast.

I'm actually considering writing two headers for it, one for C projects and another one for C++ projects, as well as compiling the library as a separate translation unit. Kinda kills the point of `inline` though...

Anyway, we now have:

- Vector 2 with floating point members
- Vector 2 with integer members (I guess I could union them together with the one above but for now I don't feel it's worth potential confusion)
- Vector 3 (floating point)
- Vector 4 (floating point)

For each one of these, I have:

- "Constructor" function
- `Add` (including +=)
- `Sub` (including -=)
- `Mul` (including \*=)
- `Div` (including /=)
- `Compare`
- `Inner` (dot product)
- `Hadamard`
- `LenSquared`
- `Length`
- `Normalize` (unit vector)

Plus I have conversions between v2f and v2i, and a `Cross` product for v3. Again, pretty happy with the set for now.

In the meantime, I started making my way through the [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html). As usual, I'm proceeding with a deliberate slowness, trying to fully understand what I'm typing in. It makes me want also to watch Casey's streams about Handmade Ray...

I also hooked the raytracer to my input system and the framerate is quite good in the optimized build! This reminds me that I'll need to implement at least a framerate indicator at one point. Not today.

![Raytracing a ball. Interesting artifacts appear now and then.](media/Day32/day32.gif)

## 32. July 20, 2020 - Understanding quadratic equations

I spent a couple hours trying to understand better the math behind the sphere hit code. I never studied vector algebra in school, so right now I'm generally struggling with what I see.

I started from looking up what `Discriminant` in the `Hit` function means, and [quickly realized](https://en.wikipedia.org/wiki/Discriminant) that it has t do exactly with the [quadratic equations](https://en.wikipedia.org/wiki/Quadratic_function). The formula for Degree 2 immediately jumped to my eyes as the same one featured in the tutorial.

Overall, I didn't understand much of it. However, it seems something simple enough to try and understand on my own. First, I'll need to return to the formula by which the sphere was calculated, and try to understand how it arrived until here.

Pretty much it. Other than that, made some refactoring and fixed a bug with the color output (color overflows).

## 33. July 25, 2020 - Quadratic Math Considerations

Last time, I tried to investigate quadratic functions a bit. The main problem is that, in the code I'm looking at, quadratic algebra is mixed with vectors, and I only have a superficial understanding of both.

At its simplest, a quadratic function is where one of its elements is... quadratic, i.e. squared (hence the name). Thefore the single-variable quadratic function has the form `ax² + bx + c = 0`, where `a` cannot be `0`. (source: [Wikipedia](https://en.wikipedia.org/wiki/Quadratic_function)). The reason for the latter is simple: if there's no a, there's no quadratic function. (Does it mean we can still have `ax² + c = 0` or `ax² + bx = 0`?). Its function is a parabola, quite easy. I do remember some of this!

Now, the function from which we left off is `t = b² - 4ac` or, in its "simplified form", `t = (b/2)² - ac`. So why does it look like this?

Let's remember that, for each point on a ray, we can test if it hits the sphere using the following formula:

![Breakdown of the sphere hit test](media/Day33/img1.png)

But we don't care about testing every single point on a ray by plugging in arbitrary `t` values (_Or do we? Note for the future..._). We want to find `t`, and, in this context, the terms can be renamed as follows:

![Renaming the terms](media/Day33/img2.png)

And adapted to programming like this:

![Programmer-friendly view](media/Day33/img3.png)

Now, let's set aside for a moment `ax²` and `c`, and let's inspect further `bx`. Here, `t` is multiplied to a member of a dot product, so let's try and see what would it take to extract it from here:

![Extracting t](media/Day33/img4.png)

We can see that algebraic equations simply allow us to extract `t` from the dot product, which finally brings us to our quadratic equation

![Final quadratic equation](media/Day33/img5.png)

where `x` is the element to be found, and `a` cannot be 0.

So far, so good. Now what is a discriminant and why are we trying to find it?

Discriminant (noted as `D` or `Δ`) is a number which determines whether a quadratic equation has 1, 2 or 0 real roots. For our purposes, it signifies how many intersects with a sphere a given ray has.

We can then, based on this information, find the actual `t`/s, if any.

For [quadratic equations](https://en.wikipedia.org/wiki/Quadratic_equation#Discriminant), discriminant has a very precise shape. For an equation `ax² + bx + c = 0` (where `a` is not 0),

![Quadratic formula](media/day33/img7.png)

If `a` _is_ 0, there's no reason for this complexity, as the answer is 1. In our case, this would happen when direction is 0, but it's not something that we would ever allow (because this would mean, for example, allowing division by 0 elsewhere), and I'm not even sure is possible.

Anyway, once you have your `D` you can go ahead and find the actual `t` by applying the ["quadratic formula"](https://en.wikipedia.org/wiki/Quadratic_formula):

![Quadratic formula](media/day33/img6.png)

where the square rooted part is actually `D`.

Shirley goes further and simplifies `D` to become

![Quadratic formula](media/day33/img8.png)

where `h` is half `b`. Ultmately, if `D` is greater than 0, `t` is calculated using the formula

![Quadratic formula](media/day33/img9.png)

And that's pretty much it. After that you might want to package your data somehow and prepare for output.

I guess it makes a bit more sense to me now. Maybe. In the meantime, I packaged this knowledge in the following manner for sphere hit code:

    v3  DirectHitVector   = Ray.Origin - Sphere->Center;
    f32 RayDirectionLenSq = LenSquared(Ray.Direction);                      // a
    f32 AngleCoefficient  = Inner(DirectHitVector, Ray.Direction);          // h
    f32 SphereNormalLenSq = LenSquared(DirectHitVector) - Sphere->RadiusSq; // c

    // find the discriminant based on formula h² - ac
    f32 Discriminant = Square(AngleCoefficient) - (RayDirectionLenSq * SphereNormalLenSq);

    if (Discriminant > 0) {
        f32 DiscRoot = SquareRoot(Discriminant);

        // Find the ray length at hit
        f32 HitPoint1 = (-AngleCoefficient - DiscRoot) * Ray.InvLenSquared;
        f32 HitPoint2 = (-AngleCoefficient + DiscRoot) * Ray.InvLenSquared;

        // Do stuff with t
    }

## 34. July 30, 2020 - Frame length timing

I am now timing frame start and end. Unfortunately, I cannot yet output the actual numbers on the screen, however I'm already tracking if we hit 60/30 fps (or more). In release mode, we're currently hitting 60fps in our ball raytracer!

Also, I solved a small mystery: PerformanceFrequency was capped at 10 Mhz. Seems like [this is intended](https://docs.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps#direct-tsc-usage), as `QueryPerformanceFrequency` shouldn't necessarily return accurate clock speeds. In any case, it still should be fine if I only use `QueryPerformanceCounter` for measuring, and I do. If necessary, more digging should be available [here](https://answers.microsoft.com/en-us/windows/forum/all/queryperformancefrequency-qpc-is-10-mhz-since/d0fb399d-5dfd-4a7a-af5f-220751953ad0).

## 35. July 31, 2020 - stb_sprintf and linked lists

I have added a piece of code that, at least for the time being, will remain relatively opaque to me: one of the Sean [Barrett](http://nothings.org/)'s single-header libraries, `stb_sprintf`. I'm fine with it, again for the time being: I trust Sean's expertise, and I know precisely what this library does. I also stripped it down (and will probably strip down even more in the future) so that it does the mininimum necessary.

Right now, I'm using it for debug stuff, i.e. showing yesterday's FPS counters, but I'm sure it'll come even more handy at some point. If I'm ever entering in the advanced string manipulation territory (which I should), I'll need to define my own string format, and have the function work with my memory, strings, etc.

Anyway, I now have my FPS counters, showing that raytracing in debug mode is SLOW (I still get 100+ fps in optimized mode though... on 225p). Much room for wide/multithreaded optimizations IF I really want to embark on this journey.

![Framerate counter, 225p](media/day35/framerate.png)

![Framerate counter, 1080p](media/day35/1080p.png)

I also finally came around implementing the memory arena. The reason for it is that, in my "weekend" raytracer I've added a linked list, a perfect excuse to finally bring over the arena. This made me realize that I already have one ready, as one of `yak_libs` I worked on late last year. Hooking it up was pretty straightforward, I spent more time figuring out how do the linked lists work again.

## 36. August 1, 2020 - Visualizing rays and adding randomness

I found an online graphing tool called [Desmos](https://www.desmos.com/) and tried to play with it to visualize the sphere raycasting function. While I don't think I found what I looked for, I definitely found [something](https://www.desmos.com/calculator/kihjluohq9). Cool tool, should play more with it. Would be even cooler if it supported vector math out of the box. Someone [did make a vector](https://www.desmos.com/calculator/rsei6rd3qe) in it but it seems super complicated.

![Rayhit "function" in Desmos](media/day36/desmos.png)

Anyway, my code contribution for today consisted in taking a few random number generator functions from [Mattias Gustavsson](https://github.com/mattiasgustavsson/libs/blob/main/docs/rnd.md). I didn't need the whole suite of functions, so I only took the few I needed. I opted for the speed over equal distribution (and some sacrifice in State size). Still need to test it.

## 37. August 2, 2020 - Multisampling and Diffuse mapping

I started looking in multisampling-based antializing. In the [guide](https://raytracing.github.io/books/RayTracingInOneWeekend.html#antialiasing), Shirley suggested using ~100 samples per pixel on our 400x225 screen. I quickly discovered that the numbers tend to explode: even on our tiny resolution, 90,000 pixels quickly become 9,000,000 samples. This both tanks the performance and destroys our memory (since we're keeping the rays in the memory for performance reasons).

While this is an interesting problem to solve, I'm starting to understand where do all the teraflops go nowadays in the "RTX-enabled" cards. I'm not even hitting "half-HD", 720p! Of course, I'm sure my math is janky and my structs are bloated... Still, it's pretty clear why we preferred raster triangle rendering for so long. To render my 90k pixel scene I'm using almost 5 gigs of memory! And it's only to store the rays!

![A cool bug I encountered while implementing multisampling. It was produced from incorrect calculation of the ray index. It's achieved by mutiplying the x index (Y * Width + X) by the sample index](media/day37/multisampling_bug.png)

I still don't have enough profiling power to be able to safely say "this thing eats most of my performance power", or "that thing might be smaller" or something. On the other hand, I'm not _really_ that concerned about it. Fun thing about Sparrow, is that it allows me to follow exercises like this, and then simply jump off this module to a next one, while bringing over the pieces I enjoyed (like the yesterday's random number generator, vector3 library or a camera system I've been slowly bulding).

Oh, I also implemented another random generator from the [same library](https://raw.githubusercontent.com/mattiasgustavsson/libs/main/rnd.h)

Lastly, I finally moved away from drawing normals to something more interesting: diffuse lighting! I did however encounter some weird bug which I'm not exactly sure what's it's caused by. Maybe because I prenormalize the ray direction? Will need to experiment more with it.

![Another cool bug I encountered while implementing diffuse lighting. This one is a bit more frustrating.](media/day37/diffuse_bug.png)

## 38. August 3, 2020 - Debugging Diffuse mapping

I started the day by chasing the yesterday's graphical bug. I soon confirmed that it's indeed called "[Shadow Acne](https://digitalrune.github.io/DigitalRune-Documentation/html/3f4d959e-9c98-4a97-8d85-7a73c26145d7.htm)" and can be pretty nasty. It seems that here finally we come to the point where the double precision arithmetic prevails over the floating point.

![Shadow acne bug in the glorious 1080p](media/day38/shadow_acne_1.png)

At the end, I cranked up the epsilon to 0.1f and moved on.

![Shadow acne with 0.01f epsilon](media/day38/shadow_acne_2.png)

Another interesting bug that I found is however more mysterious. From time to time, the rendering freaks out completely and shows an outworldly bogus output:

![Bogus output](media/day38/rendering_bug.png)

I suspect that this has to do with the way random number generator operates, or something. Honestly, I have no idea about what's going on, and why is it sporadic. In any case, this project so far has gifted us a truly marvellous collection of graphical bugs.

Going back to shadow acne, after applying a different diffuse method ("True Lambertian Reflection"), I realized that it's still present there, maybe even clearer:

![Shadow acne in Lambertian reflection](media/day38/shadow_acne_3.png)

Everything became clear once I removed the diffusing at all:

![True reflection](media/day38/shadow_acne_4.png)

It's not about the diffuse at all! We were somehow receiving data with a terrible precision. It wasn't sure if it was about float vs double anymore, but it kind of was the biggest offender so I dug deeper to investigate the bug.

Right now we are getting pure reflections and shadows by polling multiple levels of recursion depth. If I gradually increase it, we get the following progression:

![Effect of the recursion depth increase](media/day38/depth-evolution.gif)

What we see here that yes, increasing depth does improve the visual fidelity of the output, but there's more to it. There's no difference between recursion depth 20 and 1000.

Next, what we see from this is, that the sky is reflected accurately, it's the normals of the sphere which are flat-shaded until they hit the next "tier". This may be explained in the following manner:

- We use truncation somewhere and information is lost
- The floating point precision is insufficient
- ???

I tried to visualize different information that I receive with the first hit (depth = 1). Seems in order, and I understood that this trail was fine. I had only one avenue remaining: the diffuse algorithm itself.

![Hit record](media/day38/hit-record.png)

So, at the end, I realized that my random number generator was returning garbage. It _was_ a truncation error, and by adding a couple assertions the problem was quickly identified and resolved. It was a fun journey.

![Perfect Diffuse](media/day38/perfect_diffuse.png)

On a completely unrelated note, I'll have to move to compute shaders on my graphics card, if I'm to continue this journey. Already now I'm only rendering in release mode, unless I want to wait for 15 seconds+ to draw a single frame.
