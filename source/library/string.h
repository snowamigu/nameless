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

STRING_API u32 string_length(char *string);
STRING_API u32 string_copy_character(char *buffer, int character);
STRING_API u32 string_copy_integer(char *buffer, bool sign, u32 value, int base, char *representation);
STRING_API u32 string_copy_double(char *buffer, f64 value, int precision);
STRING_API u32 string_copy_void_pointer(char *buffer, void *value, int base, char *representation);
STRING_API u32 string_copy(char *buffer, char *string, u32 length);
STRING_API u32 string_format(char *buffer, char *format, va_list argument_list);

#endif