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
    int WriteConsoleA(void *console_output, char *buffer, u32 characters_count, u32 *characters_written, void *reserved);
    u32 FormatMessageA(u32 flags, void *source, u32 message_id, u32 language_id, char *buffer, u32 buffer_size, va_list *arguments);
#else
    #error "Platform not supported."
#endif

/* Converts platform error code to console error code. */
INTERNAL uhalf console_error_get(u32 error_code)
{
    uhalf error;

#ifdef WINDOWS
    switch(error_code)
    {
        case ERROR_SUCCESS:
        {
            error = CONSOLE_ERROR_SUCCESS;
        } break;

        default:
        {
            ASSERT(!"Add error to console_enumeration.");
            error = CONSOLE_ERROR;
        } break;
    }
#else
    #error "Platform not supported."
#endif

    return error;
}

/* Gives the string length. */
INTERNAL uhalf console_string_length(u32 *length, char *string)
{
    *length = 0;

    while(*string++ != '\0')
    {
        if(*length == MAX_U32)
        {
            return CONSOLE_ERROR_BUFFER_INSUFFICIENT;
        }

        (*length)++;
    }

    return CONSOLE_ERROR_SUCCESS;
}

/* Copies console error message to buffer given a console code. */
CONSOLE_API uhalf console_error_get_message(u32 error_code, char *buffer, u32 buffer_size)
{
    uhalf error;
    char *message = "?";
    u32 message_length;

    /* Special case. */
    if(buffer_size < sizeof(char))
    {
        return CONSOLE_ERROR_BUFFER_INSUFFICIENT;
    }

    /* Conversion console_error -> message. */
#ifdef WINDOWS
    switch(error_code)
    {
        case ERROR_SUCCESS:
        {
            message = "CONSOLE_ERROR_SUCCESS";
        } break;

        case CONSOLE_ERROR:
        {
            message = "CONSOLE_ERROR";
        } break;

        case CONSOLE_ERROR_INVALID_PARAMETER:
        {
            message = "CONSOLE_ERROR_INVALID_PARAMETER";
        } break;

        case CONSOLE_ERROR_BUFFER_INSUFFICIENT:
        {
            message = "CONSOLE_ERROR_BUFFER_INSUFFICIENT";
        } break;

        default:
        {
            ASSERT(!"Add error to console_enumeration.");
            message = "CONSOLE_ERROR";
        } break;
    }
#else
    #error "Platform not supported."
#endif

    /* Checks for message_length > buffer_size before copying it. */
    error = console_string_length(&message_length, message);

    if(error != CONSOLE_ERROR_SUCCESS)
    {
        return error;
    }

    if(message_length > buffer_size)
    {
        return CONSOLE_ERROR_BUFFER_INSUFFICIENT;
    }

    /* Copies message to buffer. */
    while((message != '\0') && (buffer_size > 0))
    {
        *buffer++ = *message++;
        buffer_size--;
    }

    if(buffer_size > 0)
    {
        *buffer = '\0';
    }
    else
    {
        *(--buffer) = '\0';
    }

    return CONSOLE_ERROR_SUCCESS;
}

/* Writes to the console handle device. */
CONSOLE_API uhalf console_write(u32 console_handle, char *buffer, u32 buffer_length)
{
#ifdef WINDOWS
    void *output_handle;

    if(console_handle != CONSOLE_HANDLE_OUTPUT)
    {
        return CONSOLE_ERROR_INVALID_PARAMETER;
    }

    output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if(!output_handle || (output_handle == INVALID_HANDLE_VALUE))
    {
        return console_error_get(GetLastError());
    }

    if(!WriteConsoleA(output_handle, buffer, buffer_length, 0, 0))
    {
        return console_error_get(GetLastError());
    }
#else
    #error "Platform not supported."
#endif

    return CONSOLE_ERROR_SUCCESS;
}