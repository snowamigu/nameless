#ifndef STRING_H
#define STRING_H

#include "platform.h"
#include "definitions.h"
#include <stdarg.h>

#ifdef STRING_EXPORT
    #define STRING_API EXPORT
#else
    #ifdef STRING_IMPORT
        #define STRING_API IMPORT
    #else
        #define STRING_API
    #endif
#endif

#define STRING_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define STRING_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define STRING_ABSOLUTE(x) (((x) < 0) ? (-(x)) : (x))

typedef enum
{
    STRING_ERROR_SUCCESS,
    STRING_ERROR,
    STRING_ERROR_BUFFER_INSUFFICIENT,
    STRING_ERROR_INVALID_FORMAT_SPECIFIER,
    STRING_ERROR_INVALID_NUMBER_BASE,
    STRING_ERROR_ENDIANESS_NOT_SUPPORTED,
    STRING_ERROR_INVALID_PRECISION,
    STRING_ERROR_INVALID_WIDTH
} string_enumeration;

STRING_API bool string_is_letter(int character);
STRING_API bool string_is_digit(int character);
STRING_API uhalf string_length(u32 *length, char *string);
STRING_API uhalf string_copy_character(u32 *count, char *buffer, u32 buffer_size, int character);
STRING_API uhalf string_copy_integer(u32 *count, char *buffer, u32 buffer_size, bool sign, u32 value, int representation);
STRING_API uhalf string_copy_pointer(u32 *count, char *buffer, u32 buffer_size, void *value, int representation);
STRING_API uhalf string_copy_double(u32 *count, char *buffer, u32 buffer_size, f64 value, int precision);
STRING_API uhalf string_copy(u32 *count, char *buffer, u32 buffer_size, char *string, u32 length);
STRING_API uhalf string_format(u32 *count, char *buffer, u32 buffer_size, char *format, va_list argument_list);

#endif