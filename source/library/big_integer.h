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

#define BIG_INTEGER_MAX_LIMBS_COUNT 2048

typedef struct
{
    bool sign;
    u32 limb[BIG_INTEGER_MAX_LIMBS_COUNT];
    int length;
} big_integer;

BIG_INTEGER_API void big_integer_from_ieee754(int exponent, int bias, int k, u64_emulated *fraction, big_integer *numerator, big_integer *denominator);

#endif