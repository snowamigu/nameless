#define STRING_EXPORT
#include "string.h"
#include "definitions.h"

INTERNAL void string_apply_format_flags()
{
}

INTERNAL bool string_memory_is_little_endian()
{
    unsigned int i = 1;
    unsigned char *p = (unsigned char *)&i;

    return *p == 1;
}

STRING_API bool string_is_printable(int character)
{
    return (character >= 32) && (character <= 126);
}

STRING_API u32 string_length(char *string)
{
    u32 result = 0;

    while(*string++ != '\0')
    {
        result++;
    }
    
    return result;
}

STRING_API u32 string_copy_character(char *buffer, int character)
{
    DEBUG_ASSERT(string_is_printable(character) || (character == 10) || (character == 13));

    *buffer = (char)character;

    return 1;
}


STRING_API u32 string_copy_integer(char *buffer, bool sign, u32 value, int base, char *representation)
{
    DEBUG_ASSERT((base == 8) || (base == 10) || (base == 16));

    u32 count;
    char work_buffer[32] = {0};
    char work_buffer_index = ARRAY_COUNT(work_buffer) - 2;

    count = 0;

    if(!value)
    {
        return string_copy_character(buffer, '0');
    }

    if(sign)
    {
        value = -(s32)value;
    }

    while(value)
    {
        work_buffer[work_buffer_index--] = representation[value % base];
        value /= base;
    }

    if(sign)
    {
        work_buffer[work_buffer_index] = '-';
    }
    else
    {
        work_buffer_index++;
    }

    while(work_buffer[work_buffer_index] != '\0')
    {
        count += string_copy_character(buffer + count, work_buffer[work_buffer_index]);
        work_buffer_index++;
    }

    return count;
}

STRING_API u32 string_copy_double(char *buffer, f64 value, int precision, int base, char *representation)
{
    u32 count;
    s32 integer_part;
    int i;

    integer_part = (s32)value;
    count = string_copy_integer(buffer, value < 0, integer_part, base, representation);
    count += string_copy_character(buffer + count, '.');

    if(value < 0)
    {
        value = -value;
        integer_part = -integer_part;
    }

    value -= integer_part;

    for(i = 0; i < precision; i++)
    {
        value *= 10;
        integer_part = (s32)value;
        count += string_copy_integer(buffer + count, 0, integer_part, base, representation);
        value -= integer_part;
    }

    return count;
}

STRING_API u32 string_copy_void_pointer(char *buffer, void *value, int base, char *representation)
{
    u32 count;
    u8 *byte;
    int i;

    if(string_memory_is_little_endian())
    {
        count = 0;
        byte = (u8 *)&value;

        for(i = sizeof(void *) - 1; i >= 0; i--)
        {
            if(byte[i] < base)
            {
                count += string_copy_integer(buffer + count, 0, 0, base, representation);
            }

            count += string_copy_integer(buffer + count, 0, byte[i], base, representation);
        }
    }
    else
    {
        return 0;
    }

    return count;
}

STRING_API u32 string_copy(char *buffer, char *string, u32 length)
{
    u32 count;

    count = 0;

    while(*string != '\0')
    {
        count += string_copy_character(buffer + count, *string);
        string++;
    }
    
    return count;
}

STRING_API u32 string_format(char *format, char *buffer, va_list argument_list)
{
    u32 count;
    char *octal;
    char *decimal;
    char *hexadecimal_lowercase;
    char *hexadecimal_uppercase;

    count = 0;
    octal = "01234567";
    decimal = "0123456789";
    hexadecimal_lowercase = "0123456789abcdef";
    hexadecimal_uppercase = "0123456789ABCDEF";

    while(*format != '\0')
    {
        if(*format != '%')
        {
            count += string_copy_character(buffer + count, *format);
            format++;
        }
        else
        {
            switch(*++format)
            {
                case '%':
                {
                    count += string_copy_character(buffer + count, *format);
                    format++;
                } break;

                case 'c':
                {
                    int value;
                    value = va_arg(argument_list, int);
                    count += string_copy_character(buffer + count, value);
                    format++;
                } break;

                case 'd':
                case 'i':
                {
                    s32 value;
                    value = va_arg(argument_list, s32);
                    count += string_copy_integer(buffer + count, value < 0, value, 10, decimal);
                    format++;
                } break;

                case 'f':
                {
                    f64 value;
                    value = va_arg(argument_list, f64);
                    count += string_copy_double(buffer + count, value, 17, 10, decimal);
                    format++;
                } break;

                case 'o':
                {
                    u32 value;
                    value = va_arg(argument_list, u32);
                    count += string_copy_integer(buffer + count, 0, value, 8, octal);
                    format++;
                } break;

                case 'p':
                {
                    void *value;
                    value = va_arg(argument_list, void *);
                    count += string_copy_void_pointer(buffer + count, value, 16, hexadecimal_lowercase);
                    format++;
                } break;

                case 's':
                {
                    char *value;
                    u32 length;
                    value = va_arg(argument_list, char *);
                    length = string_length(value);
                    count += string_copy(buffer + count, value, length);
                    format++;
                } break;

                case 'u':
                {
                    u32 value;
                    value = va_arg(argument_list, u32);
                    count += string_copy_integer(buffer + count, 0, value, 10, decimal);
                    format++;
                } break;

                case 'x':
                {
                    u32 value;
                    value = va_arg(argument_list, u32);
                    count += string_copy_integer(buffer + count, 0, value, 16, hexadecimal_lowercase);
                    format++;
                } break;

                case 'X':
                {
                    u32 value;
                    value = va_arg(argument_list, u32);
                    count += string_copy_integer(buffer + count, 0, value, 16, hexadecimal_uppercase);
                    format++;
                } break;

                default:
                {
                    return count;
                } break;
            }
        }

        string_apply_format_flags();
    }

    return count;
}