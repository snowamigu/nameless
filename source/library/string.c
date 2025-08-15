#define STRING_EXPORT
#include "string.h"

INTERNAL bool string_error_is_set(uhalf *error)
{
    return (!error) ? (0) : (*error != STRING_ERROR_SUCCESS);
}

INTERNAL void string_error_set(uhalf *error, uhalf error_code)
{
    if(error)
    {
        *error = error_code;
    }
}

STRING_API u32 string_length(uhalf *error, char *string)
{
    u32 result = 0;
    
    if(string_error_is_set(error))
    {
        return result;
    }

    while(*string++ != '\0')
    {
        if(result == MAX_U32)
        {
            string_error_set(error, STRING_ERROR_BUFFER_OVERRUN);
            return result;
        }

        result++;
    }

    string_error_set(error, STRING_ERROR_SUCCESS);

    return result;
}

STRING_API void string_copy_character(uhalf *error, u32 *count, char *buffer, u32 buffer_size, int character)
{
    if(string_error_is_set(error))
    {
        return;
    }

    if((buffer_size - *count) < sizeof(char))
    {
        string_error_set(error, STRING_ERROR_INSUFFICIENT_BUFFER);
        return;
    }

    buffer[(*count)++] = (char)character;
    
    string_error_set(error, STRING_ERROR_SUCCESS);

    return;
}

STRING_API void string_copy_integer(uhalf *error, u32 *count, char *buffer, u32 buffer_size, bool sign, u32 value, int representation)
{
    int base;
    char *digits;
    char work_buffer[32] = {0};
    int work_buffer_index = 0;

    if(string_error_is_set(error))
    {
        return;
    }

    if(!value)
    {
        string_copy_character(error, count, buffer, buffer_size, '0');
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
        string_error_set(error, STRING_ERROR_INVALID_NUMBER_BASE);
        return;
    }

    if(sign)
    {
        value = -(s32)value;
    }

    while(value)
    {
        work_buffer[work_buffer_index++] = digits[value % base];
        value /= base;
    }

    if(sign)
    {
        string_copy_character(error, count, buffer, buffer_size, '-');
    }

    while(--work_buffer_index >= 0)
    {
        string_copy_character(error, count, buffer, buffer_size, work_buffer[work_buffer_index]);
    }

    string_error_set(error, STRING_ERROR_SUCCESS);

    return;
}

STRING_API void string_copy(uhalf *error, u32 *count, char *buffer, u32 buffer_size, char *string, u32 length)
{

    u32 max_length;
    u32 i = 0;

    if(string_error_is_set(error))
    {
        return;
    }

    max_length = string_length(error, string);

    if(length > max_length)
    {
        string_error_set(error, STRING_ERROR_BUFFER_OVERRUN);
        return;
    }

    for(i = 0; i < length; i++)
    {
        string_copy_character(error, count, buffer, buffer_size, *string++);
    }

    string_error_set(error, STRING_ERROR_SUCCESS);

    return;
}

INTERNAL bool u64_emulated_is_zero(u64_emulated value)
{
    return !value.low && !value.high;
}

#if 0
INTERNAL void string_float64_backwards(uhalf *error, char *buffer, u32 buffer_size, f64 value, int precision, u32 *count)
{
    uhalf result = STRING_ERROR;
    uhalf ieee754_error = IEEE754_ERROR;
    uhalf big_integer_error = BIG_INTEGER_ERROR_SUCCESS;
    bool sign = 0;
    int exponent2 = 0;
    bool is_infinity = 0;
    bool is_not_a_number = 0;
    u64_emulated mantissa = {0};
    big_integer m;
    big_integer numerator;
    big_integer denominator;
    bool is_true = 0;

    if(string_error_is_set(error))
    {
        return;
    }

    if(ieee754_double_is_infinity(error, &sign, value))
    {
        char *infinity = "inf";

        if(sign)
        {
            string_copy_character(error, count, buffer, buffer_size, '-');
        }
        
        string_copy(error, count, buffer, buffer_size, infinity, string_length(error, infinity));
        return;
    }

    if(ieee754_double_is_not_a_number(error, value))
    {
        char *not_a_number = "nan";
        string_copy(error, count, buffer, buffer_size, not_a_number, string_length(error, not_a_number));
        return;
    }

    ieee754_decode_double(error, value, &sign, &exponent2, &mantissa);

    if(u64_emulated_is_zero(mantissa))
    {
        char *string = "0.";

        if(sign)
        {
            string_copy_character(error, count, buffer, buffer_size, '-');
        }
        
        string_copy(error, count, buffer, buffer_size, string, string_length(error, string));

        while(precision-- > 0)
        {
            string_copy_character(error, count, buffer, buffer_size, '0');
        }

        return;
    }

    big_integer_from_u64_emulated(error, &m, mantissa);

    if(exponent2 >= 0)
    {
        big_integer_copy(error, &m, &numerator);
        big_integer_shift_left(error, &numerator, exponent2);
        big_integer_from_u32(error, &denominator, 1);
    }
    else
    {
        big_integer one;
        big_integer_from_u32(error, &one, 1);
        big_integer_shift_left(error, &one, -exponent2);
        big_integer_from_u64_emulated(error, &numerator, mantissa);
        big_integer_copy(error, &one, &denominator);
    }

    is_true = 1;

    if(sign)
    {
        string_copy_character(error, count, buffer, buffer_size, '-');
    }

    if(memory_is_little_endian())
    {
        big_integer quotient;
        big_integer remainder;
        big_integer low_numerator;
        big_integer low_denominator;
        big_integer high_numerator;
        big_integer high_denominator;
        big_integer low_digit;
        big_integer high_digit;
        big_integer one;
        big_integer q;
        int i = 0;
        int j = 0;
        int carry;
        bool dot = 0;
        int integer_part[64];
        int integer_part_index = 0;
        int fraction_part[64];
        int fraction_part_index = 0;

        big_integer_division_remainder(&big_integer_error, &numerator, &denominator, &q, &remainder);
        big_integer_copy(&big_integer_error, &remainder, &numerator);
        big_integer_multiply_u32(&big_integer_error, &numerator, 10, &numerator);

        for(i = 0; i < (precision + 1); i++)
        {
            big_integer_division_remainder(&big_integer_error, &numerator, &denominator, &quotient, &remainder);
            fraction_part[fraction_part_index++] = quotient.limb[0] % 10;
            big_integer_copy(&big_integer_error, &remainder, &numerator);
            big_integer_multiply_u32(&big_integer_error, &numerator, 10, &numerator);
            big_integer_multiply_u32(&big_integer_error, &numerator, 2, &low_numerator);
            big_integer_from_u32(&big_integer_error, &one, 1);
            big_integer_subtract(&big_integer_error, &low_numerator, &one, &low_numerator);
            big_integer_multiply_u32(&big_integer_error, &denominator, 2, &low_denominator);
            big_integer_multiply_u32(&big_integer_error, &numerator, 2, &high_numerator);
            big_integer_add(&big_integer_error, &high_numerator, &one, &high_numerator);
            big_integer_multiply_u32(&big_integer_error, &denominator, 2, &high_denominator);
            big_integer_division_remainder(&big_integer_error, &low_numerator, &low_denominator, &quotient, &remainder);
            big_integer_multiply_u32(&big_integer_error, &remainder, 10, &low_numerator);
            big_integer_division_remainder(&big_integer_error, &high_numerator, &high_denominator, &quotient, &remainder);
            big_integer_multiply_u32(&big_integer_error, &remainder, 10, &high_numerator);
            big_integer_division_remainder(&big_integer_error, &low_numerator, &low_denominator, &low_digit, &remainder);
            big_integer_division_remainder(&big_integer_error, &high_numerator, &high_denominator, &high_digit, &remainder);
        }

        if(!q.limb[0])
        {
            integer_part[integer_part_index++] = 0;
        }
        else
        {
            while(q.limb[0] != 0)
            {
                int digit = q.limb[0] % 10;
                integer_part[integer_part_index++] = digit;
                q.limb[0] /= 10;
            }
        }
        
        while(--integer_part_index >= 0)
        {
            string_copy_character(error, count, buffer, buffer_size, '0' + integer_part[integer_part_index]);
        }

        string_copy_character(error, count, buffer, buffer_size, '.');

        for(i = 0; i < fraction_part_index; i++)
        {
            string_copy_character(error, count, buffer, buffer_size, '0' + fraction_part[i]);
        }
    }
    else
    {
        string_error_set(error, STRING_ERROR_ENDIANESS_NOT_SUPPORTED);
        return;
    }

    string_error_set(error, STRING_ERROR_SUCCESS);

    return;
}
#endif

STRING_API void string_copy_float64(uhalf *error, u32 *count, char *buffer, u32 buffer_size, f64 value, int precision)
{
    if(string_error_is_set(error))
    {
        return;
    }

#if 0
    u32 result = STRING_ERROR;
    char work_buffer[1024];
    u32 work_buffer_length = 0;

    string_float64_backwards(error, work_buffer, sizeof(work_buffer), value, precision, &work_buffer_length);
    string_copy(error, count, buffer, buffer_size, work_buffer, work_buffer_length);
#else
    u32 result = STRING_ERROR;
    s32 integer_part = (s32)value;
    int i;

    string_copy_integer(error, count, buffer, buffer_size, integer_part < 0, integer_part, 'd');

    if(string_error_is_set(error))
    {
        return;
    }

    string_copy_character(error, count, buffer, buffer_size, '.');

    if(string_error_is_set(error))
    {
        return;
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

        string_copy_integer(error, count, buffer, buffer_size, 0, integer_part, 'd');

        if(string_error_is_set(error))
        {
            return;
        }

        value -= integer_part;
    }
#endif

    return;
}

STRING_API void string_copy_pointer(uhalf *error, u32 *count, char *buffer, u32 buffer_size, void *value, int representation)
{
    u8 *byte = (u8 *)&value;
    int i = 0;

    if(string_error_is_set(error))
    {
        return;
    }

    if(1) /* @TODO: if(memory_is_little_endian()) */
    {
        for(i = sizeof(void *) - 1; i >= 0; i--)
        {
            if(byte[i] < 16)
            {
                string_copy_character(error, count, buffer, buffer_size, '0');
            }

            string_copy_integer(error, count, buffer, buffer_size, 0, byte[i], representation);
        }
    }
    else
    {
        string_error_set(error, STRING_ERROR_ENDIANESS_NOT_SUPPORTED);
        return;
    }

    string_error_set(error, STRING_ERROR_SUCCESS);

    return;
}

STRING_API void string_format(uhalf *error, u32 *count, char *buffer, u32 buffer_size, char *format, va_list argument_list)
{
    if(string_error_is_set(error))
    {
        return;
    }

    if(buffer_size < sizeof(char))
    {
        string_error_set(error, STRING_ERROR_INSUFFICIENT_BUFFER);
        return;
    }

    *buffer = '\0';
    *count = 0;

    while(*format != '\0')
    {
        if(*format != '%')
        {
            string_copy_character(error, count, buffer, buffer_size, *format);
            format++;
        }
        else
        {
            switch(*++format)
            {
                case '%':
                {
                    string_copy_character(error, count, buffer, buffer_size, *format);
                    format++;
                } break;

                case 'c':
                {
                    int value = va_arg(argument_list, int);
                    string_copy_character(error, count, buffer, buffer_size, value);
                    format++;
                } break;

                case 'd':
                case 'i':
                {
                    s32 value = va_arg(argument_list, s32);
                    string_copy_integer(error, count, buffer, buffer_size, value < 0, (u32)value, 'd');
                    format++;
                } break;

                case 'f':
                {
                    f64 value = va_arg(argument_list, f64);
                    string_copy_float64(error, count, buffer, buffer_size, value, 17);
                    format++;
                } break;

                case 'o':
                {
                    u32 value = va_arg(argument_list, u32);
                    string_copy_integer(error, count, buffer, buffer_size, 0, value, *format);
                    format++;
                } break;

                case 'p':
                {
                    void *value = va_arg(argument_list, void *);
                    string_copy_pointer(error, count, buffer, buffer_size, value, 'x');
                    format++;
                } break;

                case 's':
                {
                    char *value = va_arg(argument_list, char *);
                    string_copy(error, count, buffer, buffer_size, value, string_length(error, value));
                    format++;
                } break;

                case 'u':
                {
                    u32 value = va_arg(argument_list, u32);
                    string_copy_integer(error, count, buffer, buffer_size, 0, value, 'd');
                    format++;
                } break;

                case 'x':
                case 'X':
                {
                    u32 value = va_arg(argument_list, u32);
                    string_copy_integer(error, count, buffer, buffer_size, 0, value, *format);
                    format++;
                } break;

                default:
                {
                    string_error_set(error, STRING_ERROR_INVALID_FORMAT_SPECIFIER);
                } break;
            }
        }

        if(*error != STRING_ERROR_SUCCESS)
        {
            break;
        }
    }

    buffer[*count] = '\0';

    return;
}