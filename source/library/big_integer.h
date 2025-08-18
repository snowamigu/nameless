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

typedef enum
{
    BIG_INTEGER_ERROR_SUCCESS,
    BIG_INTEGER_ERROR,
    BIG_INTEGER_ERROR_BUFFER_OVERRUN,
    BIG_INTEGER_ERROR_BUFFER_UNDERRUN,
    BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED,
    BIG_INTEGER_ERROR_INVALID_PARAMETER,
    BIG_INTEGER_ERROR_DIVISION_BY_ZERO
} big_integer_enumeration;

#define BIG_INTEGER_MAX_LIMB_COUNT 2048

typedef struct
{
    u32 limb[BIG_INTEGER_MAX_LIMB_COUNT];
    int length;
} big_integer;

BIG_INTEGER_API uhalf big_integer_add_u32(big_integer *augend, u32 addend);
BIG_INTEGER_API uhalf big_integer_shift_left(big_integer *value, int times);
BIG_INTEGER_API uhalf big_integer_from_double(f64 value, big_integer *numerator, big_integer *denominator);
BIG_INTEGER_API uhalf big_integer_shift_right(big_integer *value, int times);
BIG_INTEGER_API uhalf big_integer_shift_left(big_integer *value, int times);
BIG_INTEGER_API uhalf big_integer_from_u32(big_integer *integer_big, u32 value);
BIG_INTEGER_API uhalf big_integer_from_u64_emulated(big_integer *integer_big, u64_emulated value);
BIG_INTEGER_API uhalf big_integer_to_integer_fraction_parts(big_integer *numerator, int *integer_part, int integer_part_count, big_integer *denominator, int *denominator_part, int denominator_part_count);
#endif