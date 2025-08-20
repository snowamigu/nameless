#define STRING_EXPORT
#include "string.h"
#include "memory.h"
#include "ieee754.h"
#include "big_integer.h"

/* Returns format specifier flags, etc. */
INTERNAL void string_format_specifiers(char **format, bool *left_align, bool *show_sign, bool *space_if_positive, bool *zero_pad, bool *alternate_form, int *width, bool *dot_found, int *precision)
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
}

/* Counts the digits given a numeber and a base. */
INTERNAL void string_count_digits_u32(int *digits, u32 value, int base)
{
    *digits = 0;

    if(!(base > 0))
    {
        return;
    }

    if(!value)
    {
        *digits = 1;
        return;
    }

    for(; value; value /= base)
    {
        (*digits)++;
    }

    return;
}

/* Advances width characters if width > 0. */
INTERNAL void string_format_specifiers_width(u32 *count, char *buffer, u32 buffer_size, char **format, int *width, bool left_align, int character_count)
{
    int w = *width;

    if(*width < 1)
    {
        return;
    }

    w = *width - character_count;

    while(w-- > 0)
    {
        string_copy_character(count, buffer, buffer_size, ' ');
    }

    *width = w;
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
STRING_API void string_length(u32 *length, char *string)
{
    *length = 0;

    while(*string++ != '\0')
    {
        if(*length == MAX_U32)
        {
            return;
        }

        (*length)++;
    }
}

/* Copies a character into buffer. */
STRING_API void string_copy_character(u32 *count, char *buffer, u32 buffer_size, int character)
{
    if(buffer_size < sizeof(char))
    {
        return;
    }

    buffer[(*count)++] = (char)character;
}

/* Copy a integer in a base into the buffer. */
STRING_API void string_copy_integer(u32 *count, char *buffer, u32 buffer_size, bool sign, u32 value, int representation)
{
    int base;
    char *digits;
    char work_buffer[32] = {0};
    int i;

    if(!value)
    {
        string_copy_character(count, buffer, buffer_size, '0');
        return;
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
        return;
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
        string_copy_character(count, buffer, buffer_size, '-');
    }

    while(--i >= 0)
    {
        string_copy_character(count, buffer, buffer_size, work_buffer[i]);
    }
}

/* Copies a pointer to the buffer. */
STRING_API void string_copy_pointer(u32 *count, char *buffer, u32 buffer_size, void *value, int representation)
{
    if(memory_is_little_endian())
    {
        u8 *byte = (u8 *)&value;
        int i;

        for(i = sizeof(void *) - 1; i >= 0; i--)
        {
            if(byte[i] < 16)
            {
                string_copy_character(count, buffer, buffer_size, '0');
            }
            
            string_copy_integer(count, buffer, buffer_size, 0, byte[i], representation);
        }
    }
    else
    {
        return;
    }
}

/**/
INTERNAL void string_big_integers_to_integer_fraction_parts(int precision, big_integer *numerator, big_integer *denominator, int *integer_part, int integer_part_count, int *fraction_part, int fraction_part_count)
{
    big_integer quotient;
    big_integer remainder;
    int i;
    int j;
    int temporary[309 + 1];

    memory_zeroed(integer_part, sizeof(int) * integer_part_count);
    memory_zeroed(fraction_part, sizeof(int) * fraction_part_count);

    big_integer_divide(numerator, denominator, &quotient, &remainder);
    
    j = 0;

    for(i = quotient.length - 1; i >= 0; i--)
    {
        temporary[j++] = quotient.limb[i];
    }

    i = 0;

    while(--j >= 0)
    {
        integer_part[i] = temporary[j];
    }

    j = 0;

    for(i = 0; i < 17; i++)
    {
        u32 digit;

        digit = big_integer_divide_by_10(&remainder);

        temporary[j++] = digit;
    }

    i = 0;

    while(--j >= 0)
    {
        fraction_part[i] = temporary[j];
    }
}

/* Copies double into buffer. */
STRING_API void string_copy_double(u32 *count, char *buffer, u32 buffer_size, f64 value, int precision)
{
#if 1
    int exponent_unbiased;
    int exponent2;
    ieee754_double ieee754;
    big_integer numerator;
    big_integer denominator;
    int integer_part[309 + 1];
    int fraction_part[17 + 1];
    int k;

    ieee754_from_double(value, &ieee754);

    /* Special cases. */

    if(ieee754.is_zero)
    {
        char *string;
        u32 length;

        string = "0.";
        
        if(ieee754.sign)
        {
            string_copy_character(count, buffer, buffer_size, '-');
        }

        string_length(&length, string);

        string_copy(count, buffer, buffer_size, string, length);

        while(precision-- > 0)
        {
            string_copy_character(count, buffer, buffer_size, '0');
        }

        return;
    }
    else if(ieee754.is_infinity)
    {
        char *string;
        u32 length;

        string = "inf";
        
        if(ieee754.sign)
        {
            string_copy_character(count, buffer, buffer_size, '-');
        }

        string_length(&length, string);

        string_copy(count, buffer, buffer_size, string, length);
        return;
    }
    else if(ieee754.is_quiet_not_a_number || ieee754.is_signaling_not_a_number)
    {
        char *string;
        u32 length;

        string = "NaN";

        if(ieee754.sign)
        {
            string_copy_character(count, buffer, buffer_size, '-');
        }
        
        if(ieee754.is_quiet_not_a_number)
        {
            string_copy_character(count, buffer, buffer_size, 'q');
        }
        else if(ieee754.is_signaling_not_a_number)
        {
            string_copy_character(count, buffer, buffer_size, 's');
        }

        string_length(&length, string);

        string_copy(count, buffer, buffer_size, string, length);
        return;
    }

    k = (ieee754.is_subnormal) ? (-1074) : (((ieee754.exponent - 1023) < 52) ? (ieee754.exponent - 1023) : (52));

    big_integer_from_ieee754(ieee754.sign, ieee754.exponent, 1023, k, &ieee754.fraction, &numerator, &denominator);

    string_big_integers_to_integer_fraction_parts(precision, &numerator, &denominator, integer_part, ARRAY_COUNT(integer_part), fraction_part, ARRAY_COUNT(fraction_part));

    k = 0;
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
}

/* Copies a string into buffer. */
STRING_API void string_copy(u32 *count, char *buffer, u32 buffer_size, char *string, u32 length)
{
    u32 i;

    for(i = 0; i < length; i++)
    {
        string_copy_character(count, buffer, buffer_size, *string);
        string++;
    }
}

/* Formats a string. */
STRING_API void string_format(u32 *count, char *buffer, u32 buffer_size, char *format, va_list argument_list)
{
    if(buffer_size < sizeof(char))
    {
        return;
    }

    *count = 0;

    while(*format != '\0')
    {
        if(*format != '%')
        {
            string_copy_character(count, buffer, buffer_size, *format);
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
            string_format_specifiers(&format, &left_align, &show_sign, &space_if_positive, &zero_pad, &alternate_form, &width, &dot_found, &precision);

            switch(*format)
            {
                /* @TODO: %e, %E, %g, %G. */
                case '%':
                {
                    string_copy_character(count, buffer, buffer_size, *format);
                    format++;
                } break;

                case 'c':
                {
                    /* '-', width. */
                    int value = va_arg(argument_list, int);

                    if(!left_align)
                    {
                        string_format_specifiers_width(count, buffer, buffer_size, &format, &width, 0, 1);
                    }

                    string_copy_character(count, buffer, buffer_size, value);
                    
                    if(left_align)
                    {
                        string_format_specifiers_width(count, buffer, buffer_size, &format, &width, left_align, 1);
                    }

                    format++;
                } break;

                case 'd':
                case 'i':
                {
                    /* @TODO: -, +, ' ', 0 (ignore if precision is given), width, precision. */
                    s32 value;
                    value = va_arg(argument_list, s32);
                    string_copy_integer(count, buffer, buffer_size, value < 0, value, 'd');
                    format++;
                } break;

                case 'f':
                {
                    /* @TODO: -, +, ' ', 0 (ignored if - present), #, width, precision. */
                    f64 value = va_arg(argument_list, f64);
                    string_copy_double(count, buffer, buffer_size, value, (precision > 0) ? (precision) : (17));
                    format++;
                } break;

                case 'o':
                {
                    /* @TODO: -, 0, #, width, precision. */
                    u32 value = va_arg(argument_list, u32);
                    string_copy_integer(count, buffer, buffer_size, 0, value, *format);
                    format++;
                } break;

                case 'p':
                {
                    void *value = va_arg(argument_list, void *);
                    string_copy_pointer(count, buffer, buffer_size, value, 'x');
                    format++;
                } break;

                case 's':
                {
                    /* @TODO: width, precision, '-'. */
                    u32 length;
                    char *value = va_arg(argument_list, char *);
                    string_length(&length, value);
                    string_copy(count, buffer, buffer_size, value, length);
                    format++;
                } break;

                case 'u':
                {
                    /* @TODO: -, 0, width, precision. */
                    u32 value = va_arg(argument_list, u32);
                    string_copy_integer(count, buffer, buffer_size, 0, value, 'd');
                    format++;
                } break;

                case 'x':
                case 'X':
                {
                    /* @TODO: -, 0, #. */
                    u32 value = va_arg(argument_list, u32);
                    string_copy_integer(count, buffer, buffer_size, 0, value, *format);
                    format++;
                } break;

                default:
                {
                } break;
            }
        }
    }

    buffer[*count] = '\0';
}