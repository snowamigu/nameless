#ifndef CONSOLE_H
#define CONSOLE_H

/* This needs to be linked against "kernel32.lib". */

#include "platform.h"
#include "definitions.h"

#ifdef CONSOLE_EXPORT
    #define CONSOLE_API EXPORT
#else
    #ifdef CONSOLE_IMPORT
        #define CONSOLE_API IMPORT
    #else
        #define CONSOLE_API
    #endif
#endif

typedef enum
{
    CONSOLE_ERROR_SUCCESS,
    CONSOLE_ERROR,
    CONSOLE_ERROR_INVALID_PARAMETER,
    CONSOLE_ERROR_BUFFER_INSUFFICIENT,
    
    CONSOLE_HANDLE_OUTPUT
} console_enumeration;

CONSOLE_API u32 console_error_get_message(u32 error_code, char *buffer, u32 buffer_size);
CONSOLE_API u32 console_write(u32 console_handle, char *buffer, u32 buffer_length);

#endif