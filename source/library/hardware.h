#ifndef HARDWARE_H
#define HARDWARE_H

#include "library/debug.h"
#include "library/release.h"
#include "library/platform.h"

#ifdef HARDWARE_EXPORT
    #define HARDWARE_API EXPORT
#else
    #ifdef HARDWARE_IMPORT
        #define HARDWARE_API IMPORT
    #else
        #define HARDWARE_API
    #endif
#endif

typedef enum
{
    HARDWARE_HANDLE_CONSOLE_OUTPUT
} hardware_enumeration;

HARDWARE_API uhalf hardware_write(u32 handle, char *buffer, u32 buffer_length);

#endif