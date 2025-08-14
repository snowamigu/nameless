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

typedef enum
{
    STRING_ERROR_SUCCESS,
    STRING_ERROR,
    STRING_ERROR_INSUFFICIENT_BUFFER,
    STRING_ERROR_INVALID_FORMAT_SPECIFIER,
    STRING_ERROR_INVALID_NUMBER_BASE,
    STRING_ERROR_ENDIANESS_NOT_SUPPORTED,
    STRING_ERROR_BUFFER_OVERRUN,
    STRING_ERROR_IEEE754    
} string_enumeration;

STRING_API uhalf string_length(u32 *length, char *string);
STRING_API void string_copy_character(uhalf *error, u32 *count, char *buffer, u32 buffer_size, int character);
STRING_API void string_copy_integer(uhalf *error, u32 *count, char *buffer, u32 buffer_size, bool sign, u32 value, int representation);
STRING_API void string_copy(uhalf *error, u32 *count, char *buffer, u32 buffer_size, char *string, u32 length);
STRING_API void string_copy_float64(uhalf *error, u32 *count, char *buffer, u32 buffer_size, f64 value, int precision);
STRING_API void string_copy_pointer(uhalf *error, u32 *count, char *buffer, u32 buffer_size, void *value, int representation);
STRING_API void string_format(uhalf *error, u32 *count, char *buffer, u32 buffer_size, char *format, va_list argument_list);

#endif