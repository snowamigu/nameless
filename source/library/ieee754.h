#ifndef IEEE754_H
#define IEEE754_H

#include "platform.h"
#include "definitions.h"

#ifdef IEEE754_EXPORT
    #define IEEE754_API EXPORT
#else
    #ifdef IEEE754_IMPORT
        #define IEEE754_API IMPORT
    #else
        #define IEEE754_API
    #endif
#endif

typedef enum
{
    IEEE754_ERROR_SUCCESS,
    IEEE754_ERROR,
    IEEE754_ERROR_ENDIANESS_NOT_SUPPORTED
} ieee754_enumeration;

typedef union
{
    f64 d;
    u32 i[2];
} ieee754_double;

#endif