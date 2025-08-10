#define CONSOLE_EXPORT
#include "console.h"
#include <stdarg.h>

#ifdef WINDOWS
    #define ERROR_SUCCESS 0
    #define STD_OUTPUT_HANDLE ((u32)-11)
    #define INVALID_HANDLE_VALUE ((void *)-1)
    #define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000
    #define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200
    
    u32 GetLastError();
    void *__stdcall GetStdHandle(u32 standard_handle);
    int __stdcall WriteConsoleA(void *console_output, char *buffer, u32 buffer_length, u32 *characteres_written, void *reserved);
    u32 FormatMessageA(u32 flags, void *source, u32 message_id, u32 language_id, char *buffer, u32 buffer_size, va_list *arguments);
#else
    #error "Platform not supported."
#endif

INTERNAL u32 console_error_get(u32 error)
{
    u32 result = CONSOLE_ERROR;

#ifdef WINDOWS
    switch(error)
    {
        case ERROR_SUCCESS:
        {
            result = CONSOLE_ERROR_SUCCESS;
        } break;

        default:
        {
            result = CONSOLE_ERROR;
            ASSERT(!"Add CONSOLE_ERROR* to console_enumeration.");
        } break;
    }
#else
    #error "Platform not supported."
#endif

    return result;
}

CONSOLE_API uhalf console_error_get_message(u32 error, char *buffer, u32 buffer_size)
{
#ifdef WINDOWS
    u32 length = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, error, 0, buffer, buffer_size, 0);

    if(!length)
    {
        return console_error_get(GetLastError());
    }
#else
    #error "Platform not supported."
#endif

    return CONSOLE_ERROR_SUCCESS;
}

CONSOLE_API uhalf console_write(u32 console_handle, char *buffer, u32 buffer_length)
{
    if(console_handle != CONSOLE_HANDLE_OUTPUT)
    {
        return CONSOLE_ERROR_INVALID_PARAMETER;
    }

#ifdef WINDOWS
    void *output_handle;

    output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if(!output_handle || (output_handle == INVALID_HANDLE_VALUE))
    {
        return console_error_get(GetLastError());
    }

    if(WriteConsoleA(output_handle, buffer, buffer_length, 0, 0))
    {
        return console_error_get(GetLastError());
    }
#else
    #error "Platform not supported."
#endif

    return CONSOLE_ERROR_SUCCESS;
}