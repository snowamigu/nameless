#ifndef CONSOLE_H
#define CONSOLE_H

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
    CONSOLE_HANDLE_OUTPUT
} console_enumeration;

CONSOLE_API void console_write(u32 console_handle, char *buffer, u32 buffer_length);

#endif