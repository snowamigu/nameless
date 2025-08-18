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
    double d;
    u32 i[2];
} ieee754_double_union;

typedef struct
{
    bool is_zero;
    bool is_infinity;
    bool is_subnormal;
    bool is_quiet_not_a_number;
    bool is_signaling_not_a_number;
    bool sign;
    int exponent;
    u64_emulated fraction;
} ieee754_double;

IEEE754_API uhalf ieee754_from_double(f64 value, ieee754_double *ieee754);

#endif