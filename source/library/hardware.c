#define HARDWARE_EXPORT
#include "hardware.h"
#include <stdarg.h>

#ifdef WINDOWS
    #define ERROR_SUCCESS 0
    #define STD_OUTPUT_HANDLE ((u32)-11)
    #define INVALID_HANDLE_VALUE ((void *)-1)
    #define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000
    #define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200

    void *__stdcall GetStdHandle(u32 standard_handle);
    int __stdcall WriteConsoleA(void *console_output, char *buffer, u32 character_count, u32 *character_written, void *reserved);
    u32 FormatMessageA(u32 flags, void *source, u32 message_id, u32 language_id, char *buffer, u32 buffer_size, va_list *arguments);
#else
    #error "Platform not supported."
#endif

HARDWARE_API uhalf hardware_write(u32 handle, char *buffer, u32 buffer_length)
{
    uhalf error;

#ifdef WINDOWS
    void *output_handle;

    error = ERROR_SUCCESS;

    if(handle != HARDWARE_HANDLE_CONSOLE_OUTPUT)
    {
        return error;
    }

    output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if(!output_handle || (output_handle == INVALID_HANDLE_VALUE))
    {
        return 0;
    }

    if(!WriteConsoleA(output_handle, buffer, buffer_length, 0, 0))
    {
        return 0;
    }
#else
    #error "Platform not supported."
#endif

    return error;
}