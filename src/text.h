#if !defined(SPARROW_TEXT_H)

#if !defined wchar_t
typedef int wchar_t;
#endif

#define STR_SHORT 128
#define STR_MEDIUM 512
#define STR_MAX 4096

struct text_buffer
{
    unsigned int Length;
    unsigned int MaxLength;
    char* Data;
};

inline unsigned int
StringLength(char* String)
{
    unsigned int Result = 0;
    while (*String++) {
        ++Result;
    }
    return (Result);
}

inline void
TextConcat(struct text_buffer* Buffer, char* NewText)
{
    unsigned int Length = StringLength(NewText);
    Assert(Buffer->Length + Length < Buffer->MaxLength);
    memcpy(Buffer->Data + Buffer->Length, NewText, Length);
    Buffer->Length += Length;
}

inline struct text_buffer
Text(char* Input, unsigned int Size, char* Buffer)
{
    struct text_buffer Result = {0};
    if (Buffer) {
        memset(Buffer, 0, Size);
        Result.Data = Buffer;
        Result.MaxLength = Size;

        if (Input) {
            TextConcat(&Result, Input);
        }
    }
    else {
        Result.Data = Input;
        Result.Length = StringLength(Input);
        Result.MaxLength = Result.Length;
    }

    return (Result);
}

#define SPARROW_TEXT_H
#endif
