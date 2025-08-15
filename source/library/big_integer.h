#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include "platform.h"
#include "definitions.h"

#ifdef BIG_INTEGER_EXPORT
    #define BIG_INTEGER_API EXPORT
#else
    #ifdef BIG_INTEGER_IMPORT
        #define BIG_INTEGER_API IMPORT
    #else
        #define BIG_INTEGER_API
    #endif
#endif

#define BIG_INTEGER_MAX_LIMB_COUNT 2048

typedef enum
{
    BIG_INTEGER_ERROR_SUCCESS,
    BIG_INTEGER_ERROR,
    BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED,
    BIG_INTEGER_BUFFER_OVERRUN,
    BIG_INTEGER_ERROR_DIVISION_BY_ZERO,
    BIG_INTEGER_ERROR_BUFFER_OVERRUN
} big_integer_enumeration;

typedef struct
{
    u32 limb[BIG_INTEGER_MAX_LIMB_COUNT];
    int length;
} big_integer;

#endif