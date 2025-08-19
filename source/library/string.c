#define STRING_EXPORT
#include "string.h"
#include "memory.h"
#include "ieee754.h"
#include "big_integer.h"

INTERNAL uhalf string_error_from_big_integer_error(uhalf big_integer_error)
{
    uhalf error;

    switch(big_integer_error)
    {
        case BIG_INTEGER_ERROR_SUCCESS:
        {
            error = STRING_ERROR_SUCCESS;
        } break;

        case BIG_INTEGER_ERROR:
        {
            error = STRING_ERROR;
        } break;

        case BIG_INTEGER_ERROR_BUFFER_OVERRUN:
        {
            error = STRING_ERROR_BUFFER_OVERRUN;
        } break;

        case BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED:
        {
            error = STRING_ERROR_ENDIANESS_NOT_SUPPORTED;
        } break;

        case BIG_INTEGER_ERROR_INVALID_PARAMETER:
        {
            error = STRING_ERROR_INVALID_PARAMETER;
        } break;

        default:
        {
            ASSERT(!"");
            error = BIG_INTEGER_ERROR;
        } break;
    }

    return error;
}

/* Returns format specifier flags, etc. */
INTERNAL uhalf string_format_specifiers(char **format, bool *left_align, bool *show_sign, bool *space_if_positive, bool *zero_pad, bool *alternate_form, int *width, bool *dot_found, int *precision)
{
    int specifier_length;

    specifier_length = 0;
    *left_align = 0;
    *show_sign = 0;
    *space_if_positive = 0;
    *zero_pad = 0;
    *alternate_form = 0;
    *width = 0;
    *dot_found = 0;
    *precision = 0;

    for(; **format != '\0'; (*format)++)
    {
        if(string_is_digit(**format) || string_is_letter(**format) || (**format == '%'))
        {
            break;
        }

        specifier_length++;

        if(**format == '-')
        {
            *left_align = 1;
            continue;
        }
        else if(**format == '+')
        {
            *show_sign = 1;
            continue;
        }
        else if(**format == ' ')
        {
            *show_sign = 1;
            continue;
        }
        else if(**format == '0')
        {
            *zero_pad = 1;
            continue;
        }
        else if(**format == '#')
        {
            *alternate_form = 1;
            continue;
        }
        else if(**format == '.')
        {
            *dot_found = 1;
            continue;
        }
    }

    if(*dot_found)
    {
        while((**format != '\0') && string_is_digit(**format))
        {
            *precision = (*precision * 10) + (**format - '0');
            (*format)++;
            specifier_length++;
        }
    }

    while((**format != '\0') && string_is_digit(**format))
    {
        *width = (*width * 10) + (**format - '0');
        (*format)++;
        specifier_length++;
    }

    return STRING_ERROR_SUCCESS;
}

/* Counts the digits given a numeber and a base. */
INTERNAL uhalf string_count_digits_u32(int *digits, u32 value, int base)
{
    *digits = 0;

    if(!(base > 0))
    {
        return STRING_ERROR_INVALID_NUMBER_BASE;
    }

    if(!value)
    {
        *digits = 1;
        return STRING_ERROR_SUCCESS;
    }

    for(; value; value /= base)
    {
        (*digits)++;
    }

    return STRING_ERROR_SUCCESS;
}

/* Advances width characters if width > 0. */
INTERNAL uhalf string_format_specifiers_width(u32 *count, char *buffer, u32 buffer_size, char **format, int *width, bool left_align, int character_count)
{
    uhalf error = STRING_ERROR_SUCCESS;
    int w = *width;

    if(*width < 1)
    {
        return error;
    }

    w = *width - character_count;

    while(w-- > 0)
    {
        error = string_copy_character(count, buffer, buffer_size, ' ');

        if(error != STRING_ERROR_SUCCESS)
        {
            return error;
        }
    }

    *width = w;

    return error;
}

/* Checks if character is a letter. */
STRING_API bool string_is_letter(int character)
{
    return ((character >= 'A') && (character <= 'Z')) || ((character >= 'a') && (character <= 'z'));
}

/* Checks if character is a digit. */
STRING_API bool string_is_digit(int character)
{
    return ((character >= '0') && (character <= '9'));
}

/* Gives the string length. */
STRING_API uhalf string_length(u32 *length, char *string)
{
    *length = 0;

    while(*string++ != '\0')
    {
        if(*length == MAX_U32)
        {
            return STRING_ERROR_BUFFER_INSUFFICIENT;
        }

        (*length)++;
    }

    return STRING_ERROR_SUCCESS;
}

/* Copies a character into buffer. */
STRING_API uhalf string_copy_character(u32 *count, char *buffer, u32 buffer_size, int character)
{
    if(buffer_size < sizeof(char))
    {
        return STRING_ERROR_BUFFER_INSUFFICIENT;
    }

    buffer[(*count)++] = (char)character;

    return STRING_ERROR_SUCCESS;
}

/* Copy a integer in a base into the buffer. */
STRING_API uhalf string_copy_integer(u32 *count, char *buffer, u32 buffer_size, bool sign, u32 value, int representation)
{
    uhalf error;
    int base;
    char *digits;
    char work_buffer[32] = {0};
    int i;

    if(!value)
    {
        return string_copy_character(count, buffer, buffer_size, '0');
    }

    if(representation == 'o')
    {
        base = 8;
        digits = "01234567";
    }
    else if(representation == 'd')
    {
        base = 10;
        digits = "0123456789";
    }
    else if(representation == 'x')
    {
        base = 16;
        digits = "0123456789abcdef";
    }
    else if(representation == 'X')
    {
        base = 16;
        digits = "0123456789ABCDEF";
    }
    else
    {
        return STRING_ERROR_INVALID_NUMBER_BASE;
    }

    if(sign)
    {
        value = -(s32)value;
    }

    i = 0;

    while(value)
    {
        work_buffer[i++] = digits[value % base];
        value /= base;
    }

    if(sign)
    {
        error = string_copy_character(count, buffer, buffer_size, '-');

        if(error != STRING_ERROR_SUCCESS)
        {
            return error;
        }
    }

    while(--i >= 0)
    {
        error = string_copy_character(count, buffer, buffer_size, work_buffer[i]);

        if(error != STRING_ERROR_SUCCESS)
        {
            return error;
        }
    }

    return STRING_ERROR_SUCCESS;
}

/* Copies a pointer to the buffer. */
STRING_API uhalf string_copy_pointer(u32 *count, char *buffer, u32 buffer_size, void *value, int representation)
{
    if(memory_is_little_endian())
    {
        uhalf error;
        u8 *byte = (u8 *)&value;
        int i;

        for(i = sizeof(void *) - 1; i >= 0; i--)
        {
            if(byte[i] < 16)
            {
                error = string_copy_character(count, buffer, buffer_size, '0');

                if(error != STRING_ERROR_SUCCESS)
                {
                    return error;
                }
            }
            
            error = string_copy_integer(count, buffer, buffer_size, 0, byte[i], representation);

            if(error != STRING_ERROR_SUCCESS)
            {
                return error;
            }
        }
    }
    else
    {
        return STRING_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return STRING_ERROR_SUCCESS;
}

/* Copies double into buffer. */
STRING_API uhalf string_copy_double(u32 *count, char *buffer, u32 buffer_size, f64 value, int precision)
{
#if 1
    uhalf error;
    int exponent_unbiased;
    int exponent2;
    ieee754_double ieee754;
    big_integer numerator;
    big_integer denominator;
    int integer_part[32];
    int fraction_part[32];

    error = ieee754_from_double(value, &ieee754);

    /* Special cases. */

    if(ieee754.is_zero)
    {
        char *string;
        u32 length;

        string = "0.";
        
        if(ieee754.sign)
        {
            error = string_copy_character(count, buffer, buffer_size, '-');
        }

        error = string_length(&length, string);

        error = string_copy(count, buffer, buffer_size, string, length);

        while(precision-- > 0)
        {
            error = string_copy_character(count, buffer, buffer_size, '0');
        }

        return error;
    }
    else if(ieee754.is_infinity)
    {
        char *string;
        u32 length;

        string = "inf";
        
        if(ieee754.sign)
        {
            error = string_copy_character(count, buffer, buffer_size, '-');
        }

        error = string_length(&length, string);

        return string_copy(count, buffer, buffer_size, string, length);
    }
    else if(ieee754.is_quiet_not_a_number || ieee754.is_signaling_not_a_number)
    {
        char *string;
        u32 length;

        string = "NaN";

        if(ieee754.sign)
        {
            error = string_copy_character(count, buffer, buffer_size, '-');
        }
        
        if(ieee754.is_quiet_not_a_number)
        {
            error = string_copy_character(count, buffer, buffer_size, 'q');
        }
        else if(ieee754.is_signaling_not_a_number)
        {
            error = string_copy_character(count, buffer, buffer_size, 's');
        }

        error = string_length(&length, string);

        return string_copy(count, buffer, buffer_size, string, length);
    }

    error = big_integer_from_ieee754(0, 0, 0, 0, 0, 0);

    

#else
    uhalf error;
    s32 integer_part = (s32)value;
    int i;

    error = string_copy_integer(count, buffer, buffer_size, integer_part < 0, integer_part, 'd');

    if(error != STRING_ERROR_SUCCESS)
    {
        return error;
    }

    error = string_copy_character(count, buffer, buffer_size, '.');

    if(error != STRING_ERROR_SUCCESS)
    {
        return error;
    }

    if(value < 0)
    {
        value = -value;
        integer_part = -integer_part; 
    }

    value -= integer_part;

    for(i = 0; i < precision; i++)
    {
        value *= 10;
        integer_part = (int)value;
        error = string_copy_integer(count, buffer, buffer_size, integer_part < 0, integer_part, 'd');

        if(error != STRING_ERROR_SUCCESS)
        {
            return error;
        }

        value -= integer_part;
    }
#endif

    return STRING_ERROR_SUCCESS;
}

/* Copies a string into buffer. */
STRING_API uhalf string_copy(u32 *count, char *buffer, u32 buffer_size, char *string, u32 length)
{
    u32 i;

    for(i = 0; i < length; i++)
    {
        uhalf error = string_copy_character(count, buffer, buffer_size, *string);

        if(error != STRING_ERROR_SUCCESS)
        {
            return error;
        }

        string++;
    }
    return STRING_ERROR_SUCCESS;
}

/* Formats a string. */
STRING_API uhalf string_format(u32 *count, char *buffer, u32 buffer_size, char *format, va_list argument_list)
{
    uhalf error = STRING_ERROR_SUCCESS;

    if(buffer_size < sizeof(char))
    {
        return STRING_ERROR_BUFFER_INSUFFICIENT;
    }

    *count = 0;

    while(*format != '\0')
    {
        if(*format != '%')
        {
            error = string_copy_character(count, buffer, buffer_size, *format);
            format++;
        }
        else
        {
            bool left_align;
            bool show_sign;
            bool space_if_positive;
            bool zero_pad;
            bool alternate_form;
            int width;
            bool dot_found;
            int precision;
            
            format++;
            error = string_format_specifiers(&format, &left_align, &show_sign, &space_if_positive, &zero_pad, &alternate_form, &width, &dot_found, &precision);

            if(error != STRING_ERROR_SUCCESS)
            {
                return error;
            }

            switch(*format)
            {
                /* @TODO: %e, %E, %g, %G. */
                case '%':
                {
                    error = string_copy_character(count, buffer, buffer_size, *format);
                    format++;
                } break;

                case 'c':
                {
                    /* '-', width. */
                    int value = va_arg(argument_list, int);

                    if(!left_align)
                    {
                        error = string_format_specifiers_width(count, buffer, buffer_size, &format, &width, 0, 1);

                        if(error != STRING_ERROR_SUCCESS)
                        {
                            break;
                        }
                    }

                    error = string_copy_character(count, buffer, buffer_size, value);

                    if(error != STRING_ERROR_SUCCESS)
                    {
                        break;
                    }
                    
                    if(left_align)
                    {
                        error = string_format_specifiers_width(count, buffer, buffer_size, &format, &width, left_align, 1);
                    }

                    format++;
                } break;

                case 'd':
                case 'i':
                {
                    /* @TODO: -, +, ' ', 0 (ignore if precision is given), width, precision. */
                    s32 value;
                    value = va_arg(argument_list, s32);
                    error = string_copy_integer(count, buffer, buffer_size, value < 0, value, 'd');
                    format++;
                } break;

                case 'f':
                {
                    /* @TODO: -, +, ' ', 0 (ignored if - present), #, width, precision. */
                    f64 value = va_arg(argument_list, f64);
                    error = string_copy_double(count, buffer, buffer_size, value, (precision > 0) ? (precision) : (17));
                    format++;
                } break;

                case 'o':
                {
                    /* @TODO: -, 0, #, width, precision. */
                    u32 value = va_arg(argument_list, u32);
                    error = string_copy_integer(count, buffer, buffer_size, 0, value, *format);
                    format++;
                } break;

                case 'p':
                {
                    void *value = va_arg(argument_list, void *);
                    error = string_copy_pointer(count, buffer, buffer_size, value, 'x');
                    format++;
                } break;

                case 's':
                {
                    /* @TODO: width, precision, '-'. */
                    u32 length;
                    char *value = va_arg(argument_list, char *);
                    error = string_length(&length, value);

                    if(error != STRING_ERROR_SUCCESS)
                    {
                        break;
                    }

                    error = string_copy(count, buffer, buffer_size, value, length);
                    format++;
                } break;

                case 'u':
                {
                    /* @TODO: -, 0, width, precision. */
                    u32 value = va_arg(argument_list, u32);
                    error = string_copy_integer(count, buffer, buffer_size, 0, value, 'd');
                    format++;
                } break;

                case 'x':
                case 'X':
                {
                    /* @TODO: -, 0, #. */
                    u32 value = va_arg(argument_list, u32);
                    error = string_copy_integer(count, buffer, buffer_size, 0, value, *format);
                    format++;
                } break;

                default:
                {
                    error = STRING_ERROR_INVALID_FORMAT_SPECIFIER;
                } break;
            }
        }

        if(error != STRING_ERROR_SUCCESS)
        {
            break;
        }
    }

    buffer[*count] = '\0';

    return error;
}