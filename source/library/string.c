#define STRING_EXPORT
#include "string.h"

typedef enum
{
    IEEE754_ERROR_SUCCESS,
    IEEE754_ERROR,
    IEEE754_ERROR_ENDIANESS_NOT_SUPPORTED
} ieee754_enumeration;

typedef union
{
    f64 d;
    u32 i[2];
} ieee754_double;

#define BIG_INTEGER_MAX_LIMB_COUNT 2048

typedef enum
{
    BIG_INTEGER_ERROR_SUCCESS = 0,
    BIG_INTEGER_ERROR,
    BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED,
    BIG_INTEGER_BUFFER_OVERRUN,
    BIG_INTEGER_ERROR_DIVISION_BY_ZERO
} big_integer_enumeration;

typedef struct
{
    u32 limb[BIG_INTEGER_MAX_LIMB_COUNT];
    u32 length;
} big_integer;

STRING_API uhalf string_length(u32 *length, char *string)
{
    *length = 0;

    while(*string++ != '\0')
    {
        if(*length == MAX_U32)
        {
            return STRING_ERROR_INSUFFICIENT_BUFFER;
        }

        (*length)++;
    }

    return STRING_ERROR_SUCCESS;
}

STRING_API uhalf string_copy_character(u32 *count, char *buffer, u32 buffer_size, int character)
{
    if((buffer_size - *count) < sizeof(char))
    {
        return STRING_ERROR_INSUFFICIENT_BUFFER;
    }

    buffer[(*count)++] = (char)character;
    
    return STRING_ERROR_SUCCESS;
}

STRING_API uhalf string_copy_integer(u32 *count, char *buffer, u32 buffer_size, bool sign, u32 value, int representation)
{
    int base;
    char *digits;
    char work_buffer[32] = {0};
    int work_buffer_index = 0;

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

    while(value)
    {
        work_buffer[work_buffer_index++] = digits[value % base];
        value /= base;
    }

    if(sign)
    {
        u32 result = string_copy_character(count, buffer, buffer_size, '-');
        
        if(result != STRING_ERROR_SUCCESS)
        {
            return result;
        }
    }

    while(--work_buffer_index >= 0)
    {
        u32 result = string_copy_character(count, buffer, buffer_size, work_buffer[work_buffer_index]);

        if(result != STRING_ERROR_SUCCESS)
        {
            return result;
        }
    }

    return STRING_ERROR_SUCCESS;
}

STRING_API uhalf string_copy(u32 *count, char *buffer, u32 buffer_size, char *string, u32 length)
{
    u32 result = STRING_ERROR;
    u32 max_length = 0;

    result = string_length(&max_length, string);

    if(result != STRING_ERROR_SUCCESS)
    {
        return result;
    }

    while(*string != '\0')
    {
        result = string_copy_character(count, buffer, buffer_size, *string++);

        if(result != STRING_ERROR_SUCCESS)
        {
            return result;
        }
    }

    return result;
}

INTERNAL bool memory_is_little_endian()
{
    unsigned int x = 1;
    unsigned char *p = (unsigned char *)&x;

    return *p == 1;
}

INTERNAL bool u64_emulated_is_zero(u64_emulated value)
{
    return !value.low && !value.high;
}

INTERNAL uhalf big_integer_is_zero(bool *is_zero, big_integer *integer_big)
{
    if(memory_is_little_endian())
    {
        *is_zero = (integer_big->length == 1) && !integer_big->limb[0];
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

INTERNAL uhalf big_integer_compare(int *comparision, big_integer *lhs, big_integer *rhs)
{
    u32 i = 0;

    if(lhs->length != rhs->length)
    {
        *comparision = (lhs->length > rhs->length) ? (1) : (-1);
        return BIG_INTEGER_ERROR_SUCCESS;
    }

    for(i = 0; i < lhs->length; i++)
    {
        if(lhs->limb[i] > rhs->limb[i])
        {
            *comparision = 1;
            return BIG_INTEGER_ERROR_SUCCESS;
        }
        else if(lhs->limb[i] < rhs->limb[i])
        {
            *comparision = -1;
            return BIG_INTEGER_ERROR_SUCCESS;
        }
    }

    *comparision = 0;

    return BIG_INTEGER_ERROR_SUCCESS;
}

INTERNAL void big_integer_zeroed(big_integer *integer_big)
{
    int i = 0;

    for(i = 0; i < BIG_INTEGER_MAX_LIMB_COUNT; i++)
    {
        integer_big->limb[i] = 0;
    }

    integer_big->length = 0;
}

INTERNAL uhalf big_integer_from_u32(big_integer *integer_big, u32 value)
{
    big_integer_zeroed(integer_big);

    if(memory_is_little_endian())
    {
        integer_big->limb[integer_big->length++] = value;
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

INTERNAL uhalf big_integer_from_u64_emulated(big_integer *integer_big, u64_emulated value)
{
    big_integer_zeroed(integer_big);

    if(memory_is_little_endian())
    {
        integer_big->limb[integer_big->length++] = value.low;
        integer_big->limb[integer_big->length++] = value.high;
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

INTERNAL uhalf big_integer_copy(big_integer *source, big_integer *destination)
{
    if(memory_is_little_endian())
    {
        u32 i = 0;

        big_integer_zeroed(destination);

        for(i = 0; i < source->length; i++)
        {
            destination->limb[i] = source->limb[i];
        }

        destination->length = source->length;
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }
    
    return BIG_INTEGER_ERROR_SUCCESS;
}

INTERNAL uhalf big_integer_shift_left(big_integer *integer_big, u32 times)
{
    if(memory_is_little_endian())
    {
        int word_size = sizeof(integer_big->limb[0]) * 8;
        int outter_shift = times / word_size;
        int inner_shift = times % word_size;
        int i = 0;
        u32 carry = 0;

        for(i = integer_big->length - 1; i >= 0; i--)
        {
            integer_big->limb[i + outter_shift] = integer_big->limb[i];
        }

        for(i = 0; i < outter_shift; i++)
        {
            integer_big->limb[i] = 0;
        }

        integer_big->length += outter_shift;

        for(i = 0; i < word_size; i++)
        {
            carry = (integer_big->limb[i] >> (word_size - inner_shift)) & ((1 << (word_size - inner_shift)) - 1);
            u32 limb = (integer_big->limb[i] << inner_shift) & MAX_U32;
            integer_big->limb[i] = limb | carry;
        }

        if(carry != 0)
        {
            if(integer_big->length < BIG_INTEGER_MAX_LIMB_COUNT)
            {
                integer_big->limb[integer_big->length++] = carry;
            }
            else
            {
                return BIG_INTEGER_BUFFER_OVERRUN;
            }
        }
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

INTERNAL uhalf big_integer_division_remainder(big_integer *numerator, big_integer *denominator, big_integer *quotient, big_integer *remainder)
{
    uhalf result = BIG_INTEGER_ERROR;
    bool is_zero = 0;

    result = big_integer_is_zero(&is_zero, denominator);

    if(result != BIG_INTEGER_ERROR_SUCCESS)
    {
        return result;
    }

    if(is_zero)
    {
        return BIG_INTEGER_ERROR_DIVISION_BY_ZERO;
    }

    big_integer_zeroed(quotient);
    big_integer_zeroed(remainder);

    if(memory_is_little_endian())
    {
        u64_emulated r = {0};
        u32 i;

        for(i = numerator->length - 1; i >= 0; i--)
        {
            r.low = numerator->limb[i];
        }
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

INTERNAL uhalf ieee754_get_error_message(u32 error, char *buffer, u32 buffer_size)
{
    uhalf result = IEEE754_ERROR_SUCCESS;
    uhalf result2 = IEEE754_ERROR;
    char *string = "?";
    u32 count = 0;
    u32 length = 0;

    switch(error)
    {
        case IEEE754_ERROR_SUCCESS:
        {
            string = "IEEE754_ERROR_SUCCESS";
        } break;

        case IEEE754_ERROR:
        {
            string = "IEEE754_ERROR";
        } break;
        
        case IEEE754_ERROR_ENDIANESS_NOT_SUPPORTED:
        {
            string = "IEEE754_ERROR_ENDIANESS_NOT_SUPPORTED";
        } break;
        
        default:
        {
            ASSERT(!"Add to IEEE754_ERROR* in ieee754_enumeration.");
            result = IEEE754_ERROR;
        } break;
    }

    result2 = string_length(&length, string);

    if(result == STRING_ERROR_SUCCESS)
    {
        result2 = string_copy(&count, buffer, buffer_size, string, length);
    }

    return (result != STRING_ERROR_SUCCESS) ? (result) : (result2);
}

INTERNAL uhalf ieee754_double_is_infinity(bool *sign, bool *is_infinity, f64 value)
{
    ieee754_double u = {0};
    u32 low = 0;
    u32 high = 0;
    u32 exponent = 0;
    
    u.d = value;

    if(memory_is_little_endian())
    {
        high = u.i[1];
        *sign = (high >> 31) & 1;
        exponent = (high >> 20) & 0x7FF;
        high &= 0xFFFFF;
        low = u.i[0];

        *is_infinity = (exponent == 0x7FF) && !low && !high;
    }
    else
    {
        return IEEE754_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return IEEE754_ERROR_SUCCESS;
}

INTERNAL uhalf ieee754_double_is_not_a_number(bool *is_not_a_number, f64 value)
{
    ieee754_double u = {0};
    u32 low = 0;
    u32 high = 0;
    u32 exponent = 0;
    
    u.d = value;

    if(memory_is_little_endian())
    {
        high = u.i[1];
        exponent = (high >> 20) & 0x7FF;
        high &= 0xFFFFF;
        low = u.i[0];

        *is_not_a_number = (exponent == 0x7FF) && (low != 0) && (high != 0);
    }
    else
    {
        return IEEE754_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return IEEE754_ERROR_SUCCESS;
}

INTERNAL uhalf ieee754_decode_double(bool *sign, int *exponent2, f64 value, u64_emulated *mantissa)
{
    ieee754_double u = {0};
    u32 low = 0;
    u32 high = 0;
    int biased_exponent = 0;

    u.d = value;

    if(memory_is_little_endian())
    {    
        u.d = value;
        low = u.i[0];
        high = u.i[1];
    }
    else
    {
        return IEEE754_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    *sign = (high >> 31) & 1;
    biased_exponent = (high >> 20) & 0x7FF;
    mantissa->low = low;
    mantissa->high = high & 0xFFFFF;

    if(!biased_exponent)
    {
        if(u64_emulated_is_zero(*mantissa))
        {
            *exponent2 = -1023 - 52;
            mantissa->low = mantissa->high = 0;
        }
        else
        {
            *exponent2 = -1022 - 52;
        }
    }
    else if(biased_exponent == 0x7FF)
    {
        *exponent2 = 0 - 52;
        mantissa->low = mantissa->high = 0;
    }
    else
    {
        *exponent2 = biased_exponent - 1023 - 52;
        mantissa->high += 1 << 20;
    }

    return IEEE754_ERROR_SUCCESS;
}

INTERNAL uhalf string_float64_backwards(u32 *count, char *buffer, u32 buffer_size, f64 value, int precision)
{
    uhalf result = STRING_ERROR;
    uhalf ieee754_error = IEEE754_ERROR;
    char integer_part[64] = {0};
    char fraction_part[64] = {0};
    bool sign = 0;
    int exponent2 = 0;
    bool is_infinity = 0;
    bool is_not_a_number = 0;
    u64_emulated mantissa = {0};
    big_integer m;
    big_integer numerator;
    big_integer denominator;
    bool is_true = 0;

    *buffer = '\0';

    ieee754_error = ieee754_double_is_infinity(&sign, &is_infinity, value);

    if(ieee754_error != IEEE754_ERROR_SUCCESS)
    {
        ieee754_get_error_message(ieee754_error, buffer, buffer_size);
        return STRING_ERROR_IEEE754;
    }

    if(is_infinity)
    {
        char *infinity = "inf";
        u32 length = 0;

        if(sign)
        {
            result = string_copy_character(count, buffer, buffer_size, '-');

            if(result != STRING_ERROR_SUCCESS)
            {
                return result;
            }
        }

        result = string_length(&length, infinity);

        if(result != STRING_ERROR_SUCCESS)
        {
            return result;
        }

        return string_copy(count, buffer, buffer_size, infinity, length);
    }

    ieee754_error = ieee754_double_is_not_a_number(&is_not_a_number, value);

    if(ieee754_error != IEEE754_ERROR_SUCCESS)
    {
        ieee754_get_error_message(ieee754_error, buffer, buffer_size);
        return STRING_ERROR_IEEE754;
    }

    if(is_not_a_number)
    {
        char *not_a_number = "nan";
        u32 length = 0;
        result = string_length(&length, not_a_number);

        if(result != STRING_ERROR_SUCCESS)
        {
            return result;
        }

        return string_copy(count, buffer, buffer_size, not_a_number, length);
    }

    result = ieee754_decode_double(&sign, &exponent2, value, &mantissa);

    if(result != STRING_ERROR_SUCCESS)
    {
        return result;
    }

    if(u64_emulated_is_zero(mantissa))
    {
        char *string = "0.";
        u32 length = 0;

        result = string_length(&length, string);

        if(result != STRING_ERROR_SUCCESS)
        {
            return result;
        }

        result = string_copy(count, buffer, buffer_size, string, length);

        if(result != STRING_ERROR_SUCCESS)
        {
            return result;
        }

        while(precision-- > 0)
        {
            result = string_copy_character(count, buffer, buffer_size, '0');

            if(result != STRING_ERROR_SUCCESS)
            {
                return result;
            }
        }

        return STRING_ERROR_SUCCESS;
    }

    big_integer_from_u64_emulated(&m, mantissa);

    if(exponent2 >= 0)
    {
        result = big_integer_copy(&m, &numerator);

        if(result != BIG_INTEGER_ERROR_SUCCESS)
        {
            return result;
        }

        result = big_integer_shift_left(&numerator, exponent2);

        if(result != BIG_INTEGER_ERROR_SUCCESS)
        {
            return result;
        }

        result = big_integer_from_u32(&denominator, 1);

        if(result != BIG_INTEGER_ERROR_SUCCESS)
        {
            return result;
        }
    }
    else
    {
        big_integer one;
        result = big_integer_from_u32(&one, 1);

        if(result != BIG_INTEGER_ERROR_SUCCESS)
        {
            return result;
        }

        result = big_integer_shift_left(&one, -exponent2);

        if(result != BIG_INTEGER_ERROR_SUCCESS)
        {
            return result;
        }

        result = big_integer_from_u64_emulated(&numerator, mantissa);

        if(result != BIG_INTEGER_ERROR_SUCCESS)
        {
            return result;
        }

        result = big_integer_copy(&one, &denominator);

        if(result != BIG_INTEGER_ERROR_SUCCESS)
        {
            return result;
        }
    }

    is_true = 1;

    while(is_true)
    {
        big_integer quotient;
        big_integer remainder;

        big_integer_division_remainder(&numerator, &denominator, &quotient, &remainder);
        
        is_true = 0;
    }

    /*@TODO: more... */

    return STRING_ERROR_SUCCESS;
}

STRING_API uhalf string_copy_float64(u32 *count, char *buffer, u32 buffer_size, f64 value, int precision)
{
#if 1
    u32 result = STRING_ERROR;
    char work_buffer[1024];
    u32 work_buffer_length = 0;

    result = string_float64_backwards(&work_buffer_length, work_buffer, sizeof(work_buffer), value, precision);

    if(result != STRING_ERROR_SUCCESS)
    {
        return result;
    }
    
    result = string_copy(count, buffer, buffer_size, work_buffer, work_buffer_length);

    if(result != STRING_ERROR_SUCCESS)
    {
        return result;
    }
    
#else
    u32 result = STRING_ERROR;
    s32 integer_part = (s32)value;
    int i;

    result = string_copy_integer(count, buffer, buffer_size, integer_part < 0, integer_part, 'd');

    if(result != STRING_ERROR_SUCCESS)
    {
        return result;
    }

    result = string_copy_character(count, buffer, buffer_size, '.');

    if(result != STRING_ERROR_SUCCESS)
    {
        return result;
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

        result = string_copy_integer(count, buffer, buffer_size, 0, integer_part, 'd');

        if(result != STRING_ERROR_SUCCESS)
        {
            return result;
        }

        value -= integer_part;
    }
#endif

    return result;
}

STRING_API uhalf string_copy_pointer(u32 *count, char *buffer, u32 buffer_size, void *value, int representation)
{
    u8 *byte = (u8 *)&value;
    int i = 0;

    if(memory_is_little_endian())
    {
        for(i = sizeof(void *) - 1; i >= 0; i--)
        {
            u32 result = STRING_ERROR;

            if(byte[i] < 16)
            {
                result = string_copy_character(count, buffer, buffer_size, '0');

                if(result != STRING_ERROR_SUCCESS)
                {
                    return result;
                }
            }

            result = string_copy_integer(count, buffer, buffer_size, 0, byte[i], representation);

            if(result != STRING_ERROR_SUCCESS)
            {
                return result;
            }
        }
    }
    else
    {
        ASSERT(!"Platform endianess not supported.");
        return STRING_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return STRING_ERROR_SUCCESS;
}

STRING_API uhalf string_format(u32 *count, char *buffer, u32 buffer_size, char *format, va_list argument_list)
{
    uhalf result = STRING_ERROR;

    if(buffer_size < sizeof(char))
    {
        return STRING_ERROR_INSUFFICIENT_BUFFER;
    }

    *count = 0;

    while(*format != '\0')
    {
        if(*format != '%')
        {
            result = string_copy_character(count, buffer, buffer_size, *format);
            format++;
        }
        else
        {
            switch(*++format)
            {
                case '%':
                {
                    result = string_copy_character(count, buffer, buffer_size, *format);
                    format++;
                } break;

                case 'c':
                {
                    int value = va_arg(argument_list, int);
                    result = string_copy_character(count, buffer, buffer_size, value);
                    format++;
                } break;

                case 'd':
                case 'i':
                {
                    s32 value = va_arg(argument_list, s32);
                    result = string_copy_integer(count, buffer, buffer_size, value < 0, (u32)value, 'd');
                    format++;
                } break;

                case 'f':
                {
                    f64 value = va_arg(argument_list, f64);
                    result = string_copy_float64(count, buffer, buffer_size, value, 17);
                    format++;
                } break;

                case 'o':
                {
                    u32 value = va_arg(argument_list, u32);
                    result = string_copy_integer(count, buffer, buffer_size, 0, value, *format);
                    format++;
                } break;

                case 'p':
                {
                    void *value = va_arg(argument_list, void *);
                    result = string_copy_pointer(count, buffer, buffer_size, value, 'x');
                    format++;
                } break;

                case 's':
                {
                    u32 length = 0;
                    char *value = va_arg(argument_list, char *);
                    result = string_length(&length, value);

                    if(result == STRING_ERROR_SUCCESS)
                    {
                        result = string_copy(count, buffer, buffer_size, value, length);
                        format++;
                    }
                } break;

                case 'u':
                {
                    u32 value = va_arg(argument_list, u32);
                    result = string_copy_integer(count, buffer, buffer_size, 0, value, 'd');
                    format++;
                } break;

                case 'x':
                case 'X':
                {
                    u32 value = va_arg(argument_list, u32);
                    result = string_copy_integer(count, buffer, buffer_size, 0, value, *format);
                    format++;
                } break;

                default:
                {
                    return STRING_ERROR_INVALID_FORMAT_SPECIFIER;
                } break;
            }
        }

        if(result != STRING_ERROR_SUCCESS)
        {
            break;
        }
    }

    buffer[*count] = '\0';

    return result;
}