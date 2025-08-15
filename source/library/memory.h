#ifndef MEMORY_H
#define MEMORY_H

#include "platform.h"
#include "definitions.h"

#ifdef MEMORY_EXPORT
    #define MEMORY_API EXPORT
#else
    #ifdef MEMORY_IMPORT
        #define MEMORY_API IMPORT
    #else
        #define MEMORY_API
    #endif
#endif

typedef enum
{
    MEMORY_ERROR_SUCCESS,
    MEMORY_ERROR,
    MEMORY_ERROR_INDEX_NOT_FOUND,
} memory_enumeration;

#endif