#define CONSOLE_EXPORT
#include "console.h"
#include <stdarg.h>

#ifdef WINDOWS

#define STD_OUTPUT_HANDLE  ((u32)-11)
#define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000
#define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200
#define INVALID_HANDLE_VALUE ((void *)-1)
void *__stdcall GetStdHandle(u32 standard_handle);
int __stdcall WriteConsoleA(void *console_output, char *buffer, u32 characters_count, u32 *characters_written, void *reserved);
u32 FormatMessageA(u32 flags, void *source, u32 message_id, u32 language_id, char *buffer, u32 buffer_size, va_list *arguments);

#else
    #error
#endif

CONSOLE_API void console_write(u32 console_handle, char *buffer, u32 buffer_length)
{
    ASSERT(console_handle == CONSOLE_HANDLE_OUTPUT);

#ifdef WINDOWS
    void *output_handle;

    output_handle = GetStdHandle(STD_OUTPUT_HANDLE);


    if(!output_handle || (output_handle == INVALID_HANDLE_VALUE))
    {
        return;
    }

    if(WriteConsoleA(output_handle, buffer, buffer_length, 0, 0))
    {
        return;
    }
#else
    #error
#endif
}