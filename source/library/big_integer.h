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

BIG_INTEGER_API void big_integer_divide(big_integer *value1, big_integer *value2, big_integer *quotient, big_integer *remainder);
BIG_INTEGER_API void big_integer_from_ieee754(bool sign, int exponent, int bias, int k, u64_emulated *fraction, big_integer *numerator, big_integer *denominator);
BIG_INTEGER_API void big_integer_from_u32(u32 value1, big_integer *value2);
BIG_INTEGER_API void big_integer_multiply(big_integer *multiplier, big_integer *multiplicand, big_integer *product);
BIG_INTEGER_API bool big_integer_is_zero(big_integer *value);
BIG_INTEGER_API u32 big_integer_divide_by_10(big_integer *dividend);
BIG_INTEGER_API void big_integer_copy(big_integer *source, big_integer *destination);
#endif