# Notes on Handmade Hero
I'm rewatching some of Handmade Hero at the beginning. Here below are some my notes in regard. 

## HH Day 3

11:00 There's no reason for RAII in window code since Windows is taking care of it.
14:40 "This is a global for now, one day we're gonna move it." suuure casey. Still not sure what exactly can be done instead of it.
19:54 "Static also defines a function to be local to the [Translation unit] it's in".
21:00 "static variables are zeroed out automatically"
22:40 "GDI is Windows' graphic API, and we want to interface with it as little as possible"
23:51 "DIB stands for 'Device-independent Bitmap'"

So far the workflow is:
1. Introduce main windows callback function, with referring back to DefWindowProc by default
2. Introduce and register Window Class, link callback function
3. Introduce and show Window
4. Create main loop
5. Callback function: handle WM_CLOSE, WM_DESTROY
6. Callback function @ WM_SIZE: create frame buffer

## HH Day 4

So overall the process changed slightly. As of now, the program was working only when it received specific windows messages: WM_PAINT, WM_RESIZE namely. Also we were creating a device context to get access to the bitmap memory. 

The latter is not really necessary. You can just allocate some memory yourself, fill it out and throw into the blitter (for now it's StretchDIBits). Provided the bitmapinfo is filled out appropriately, it will be read correctly anyway. 

