#if !defined(FAKE_WINDOWS)
#ifndef _WINDOWS_

// NOTE: Entry handles
#define HINSTANCE void*

// NOTE: Memory
#define MEM_RESERVE 0x2000
#define MEM_COMMIT 0x1000
#define PAGE_READWRITE 0x4
void* __stdcall VirtualAlloc(void* address, memory_index size, u32 type, u32 protect);

// NOTE: Debug calls
#if defined (SPARROW_DEV)
void __stdcall OutputDebugStringA(char* string);
#else
#define OutputDebugStringA
#endif

#endif // !WINDOWS
#define FAKE_WINDOWS
#endif
