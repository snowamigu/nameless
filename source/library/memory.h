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
    MEMORY_ERROR_SUCCESS = ERROR_SUCCESS,
    MEMORY_ERROR = ERROR,
    MEMORY_ERROR_INVALID_PARAMETER = ERROR_INVALID_PARAMETER
} memory_enumeration;

MEMORY_API bool memory_is_little_endian();
MEMORY_API uhalf memory_zeroed(void *value, u32 size);
MEMORY_API int memory_count_trailing_zeros_u32(u32 value);
MEMORY_API int memory_count_trailing_zeros_u64_emulated(u64_emulated value);
MEMORY_API int memory_count_leading_zeros_u32(u32 value);

#endif