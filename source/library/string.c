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
    BIG_INTEGER_ERROR_SUCCESS,
    BIG_INTEGER_ERROR,
    BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED,
    BIG_INTEGER_BUFFER_OVERRUN,
    BIG_INTEGER_ERROR_DIVISION_BY_ZERO,
    BIG_INTEGER_ERROR_BUFFER_OVERRUN
} big_integer_enumeration;

typedef struct
{
    u32 limb[BIG_INTEGER_MAX_LIMB_COUNT];
    int length;
} big_integer;

typedef enum
{
    MEMORY_ERROR_SUCCESS,
    MEMORY_ERROR,
    MEMORY_ERROR_INDEX_NOT_FOUND,
} memory_enumeration;

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

INTERNAL bool big_integer_error_is_set(uhalf *error)
{
    return (!error) ? (0) : (*error != BIG_INTEGER_ERROR_SUCCESS);
}

INTERNAL void big_integer_error_set(uhalf *error, uhalf error_code)
{
    if(error)
    {
        *error = error_code;
    }
}

INTERNAL bool big_integer_is_zero(uhalf *error, big_integer *integer_big)
{
    bool result = 0;
    
    if(big_integer_error_is_set(error))
    {
        return result;
    }

    if(memory_is_little_endian())
    {
        result = (integer_big->length == 1) && !integer_big->limb[0];
    }
    else
    {
        big_integer_error_set(error, BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED);
        return result;
    }

    big_integer_error_set(error, BIG_INTEGER_ERROR_SUCCESS);

    return result;
}

INTERNAL void big_integer_normalized(uhalf *error, big_integer *integer_big)
{
    if(memory_is_little_endian())
    {
        while((integer_big->length > 1) && !integer_big->limb[integer_big->length - 1])
        {
            integer_big->length--;
        }
    }
    else
    {
        big_integer_error_set(error, BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED);
        return;
    }

    big_integer_error_set(error, BIG_INTEGER_ERROR_SUCCESS);
}

INTERNAL int big_integer_compare(uhalf *error, big_integer *lhs, big_integer *rhs)
{
    int i = 0;

    if(big_integer_error_is_set(error))
    {
        return 0;
    }

    if(lhs->length != rhs->length)
    {
        return (lhs->length > rhs->length) ? (1) : (-1);
    }

    if(memory_is_little_endian())
    {
        for(i = lhs->length - 1; i >= 0; i--)
        {
            if(lhs->limb[i] > rhs->limb[i])
            {
                return 1;
            }
            else if(lhs->limb[i] < rhs->limb[i])
            {
                return -1;
            }
        }
    }
    else
    {
        big_integer_error_set(error, BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED);
        return 0;
    }

    big_integer_error_set(error, BIG_INTEGER_ERROR_SUCCESS);

    return 0;
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

INTERNAL void big_integer_from_u32(uhalf *error, big_integer *integer_big, u32 value)
{
    if(big_integer_error_is_set(error))
    {
        return;
    }

    big_integer_zeroed(integer_big);

    if(memory_is_little_endian())
    {
        integer_big->limb[integer_big->length++] = value;
    }
    else
    {
        big_integer_error_set(error,  BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED);
        return;
    }

    big_integer_error_set(error, BIG_INTEGER_ERROR_SUCCESS);

    return;
}

INTERNAL void big_integer_from_u64_emulated(uhalf *error, big_integer *integer_big, u64_emulated value)
{
    if(big_integer_error_is_set(error))
    {
        return;
    }

    big_integer_zeroed(integer_big);

    if(memory_is_little_endian())
    {
        integer_big->limb[integer_big->length++] = value.low;
        integer_big->limb[integer_big->length++] = value.high;
    }
    else
    {
        big_integer_error_set(error, BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED);
        return;
    }

    big_integer_error_set(error, BIG_INTEGER_ERROR_SUCCESS);

    return;
}

INTERNAL void big_integer_copy(uhalf *error, big_integer *source, big_integer *destination)
{
    if(big_integer_error_is_set(error))
    {
        return;
    }

    if(memory_is_little_endian())
    {
        int i = 0;

        big_integer_zeroed(destination);

        for(i = 0; i < source->length; i++)
        {
            destination->limb[i] = source->limb[i];
        }

        destination->length = source->length;
    }
    else
    {
        big_integer_error_set(error, BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED);
        return;
    }

    big_integer_error_set(error, BIG_INTEGER_ERROR_SUCCESS);
    
    return;
}

INTERNAL void big_integer_shift_left(uhalf *error, big_integer *integer_big, u32 times)
{
    if(big_integer_error_is_set(error))
    {
        return;
    }

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
        big_integer_normalized(error, integer_big);

        for(i = 0; i < word_size; i += 32)
        {
            u32 limb = (integer_big->limb[i] << inner_shift) & MAX_U32;
            integer_big->limb[i] = limb + carry;
            carry = (integer_big->limb[i] >> (word_size - inner_shift)) & ((1 << (word_size - inner_shift)) - 1);
        }

        if(carry != 0)
        {
            if(integer_big->length < BIG_INTEGER_MAX_LIMB_COUNT)
            {
                integer_big->limb[integer_big->length++] = carry;
            }
            else
            {
                big_integer_error_set(error, BIG_INTEGER_BUFFER_OVERRUN);
                return;
            }
        }
    }
    else
    {
        big_integer_error_set(error, BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED);
        return;
    }

    big_integer_error_set(error, BIG_INTEGER_ERROR_SUCCESS);
    
    return;
}

INTERNAL void big_integer_shift_right(uhalf *error, big_integer *integer_big, int times)
{
    int word_size = sizeof(integer_big->limb[0]) * 8;
    int limb_shift = times / word_size;
    int bit_shift = times % word_size;
    int i;

    for(i = integer_big->length - 1; i > 0; i--)
    {
        integer_big->limb[i - 1] = integer_big->limb[i];
    }
}

INTERNAL void big_integer_add(uhalf *error, big_integer *lhs, big_integer *rhs, big_integer *result)
{
    int i = 0;
    u32 carry = 0;
    int max_length = (lhs->length > rhs->length) ? (lhs->length) : (rhs->length);
    big_integer r;

    big_integer_zeroed(&r);

    if(memory_is_little_endian())
    {
        for(i = 0; i < max_length; i++)
        {
            u32 lhs_limb = (!lhs->limb[i]) ? (0) : (lhs->limb[i]);
            u32 rhs_limb = (!rhs->limb[i]) ? (0) : (rhs->limb[i]);
            u32 sum = lhs_limb + rhs_limb;
            u32 inner_carry = 0;

            if(sum < lhs_limb)
            {
                inner_carry = 1;
            }

            sum += carry;

            if(sum < carry)
            {
                inner_carry = 1;
            }

            r.limb[i] = sum;
            carry = (inner_carry) ? (1) : (0);
        }

        r.length = i;

        if(carry != 0)
        {
            if(r.length < BIG_INTEGER_MAX_LIMB_COUNT)
            {
                r.limb[r.length++] = carry;
            }
            else
            {
                big_integer_error_set(error, BIG_INTEGER_ERROR_BUFFER_OVERRUN);
                return;
            }
        }
    }

    big_integer_copy(error, &r, result);
    big_integer_error_set(error, BIG_INTEGER_ERROR_SUCCESS);
    
    return;
}

INTERNAL void big_integer_subtract(uhalf *error, big_integer *lhs, big_integer *rhs, big_integer *result)
{
    u32 carry = 0;
    int i = 0;
    big_integer r;

    big_integer_zeroed(&r);

    if(memory_is_little_endian())
    {
        for(i = 0; i < lhs->length; i++)
        {
            u32 temporary = ((i < rhs->length) ? (rhs->limb[i]) : (0));
            r.limb[i] = (lhs->limb[i] - temporary - carry) & MAX_U32;
            carry = (lhs->limb[i] < (temporary + carry)) ? (1) : (0);
        }

        r.length = i;

        if(carry != 0)
        {
            if(r.length < BIG_INTEGER_MAX_LIMB_COUNT)
            {
                r.limb[r.length++] = carry;
            }
            else
            {
                big_integer_error_set(error, BIG_INTEGER_ERROR_BUFFER_OVERRUN);
                return;
            }
        }

        big_integer_normalized(error, &r);
        big_integer_copy(error, &r, result);
    }
    else
    {
        big_integer_error_set(error, BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED);
        return;
    }

    big_integer_error_set(error, BIG_INTEGER_ERROR_SUCCESS);
    return;
}

INTERNAL void big_integer_multiply_u32(uhalf *error, big_integer *lhs, u32 rhs, big_integer *result)
{
    u32 carry = 0;
    int i = 0;
    
    if(big_integer_error_is_set(error))
    {
        return;
    }

    for(i = 0; i < lhs->length; i++)
    {
        u64_emulated product = {0};
        u32 lhs_low = lhs->limb[i] & MAX_U16;
        u32 lhs_high = (lhs->limb[i] >> 16) & MAX_U16;
        u32 rhs_low = rhs & MAX_U16;
        u32 rhs_high = (rhs >> 16) & MAX_U16;
        u32 p0 = lhs_low * rhs_low;
        u32 p1 = lhs_low * rhs_high;
        u32 p2 = lhs_high * rhs_low;
        u32 p3 = lhs_high * rhs_high;
        u32 carry_p0 = p0 >> 16;
        u32 middle = p1 + p2;
        u32 carry_middle = (middle < p1) ? (1) : (0);
        
        middle += carry_p0;
        carry_middle += (middle < carry_p0) ? (1) : (0);
        product.low = (p0 & MAX_U16) | ((middle & MAX_U16) << 16);
        result->limb[i] = product.low + carry;
        carry_middle += (product.low < carry) ? (1) : (0);
        carry = p3 + ((middle >> 16) & MAX_U16) + carry_middle;
    }

    result->length = i;

    if(carry)
    {
        if(result->length < BIG_INTEGER_MAX_LIMB_COUNT)
        {
            result->limb[result->length++] = carry;
        }
        else
        {
            big_integer_error_set(error, BIG_INTEGER_ERROR_BUFFER_OVERRUN);
            return;
        }
    }

    big_integer_error_set(error, BIG_INTEGER_ERROR_SUCCESS);
    return;
}

INTERNAL bool memory_error_is_set(uhalf *error)
{
    return (!error) ? (0) : (*error != MEMORY_ERROR_SUCCESS);
}

INTERNAL void memory_error_set(uhalf *error, u32 error_code)
{
    if(error)
    {
        *error = error_code;
    }
}

INTERNAL int memory_count_leading_zeros(uhalf *error, u32 value)
{
    bool found = 0;
    int i;

    if(memory_error_is_set(error))
    {
        return 0;
    }

    for(i = 31; i >= 0; i--)
    {

        if(!(value & (1 << i)))
        {
            memory_error_set(error, MEMORY_ERROR_SUCCESS);
            return i;
        }
    }

    memory_error_set(error, MEMORY_ERROR_INDEX_NOT_FOUND);
    return 0;
}

INTERNAL void big_integer_division_remainder(uhalf *error, big_integer *numerator, big_integer *denominator, big_integer *quotient, big_integer *remainder)
{
    big_integer n;
    big_integer d;
    big_integer q;
    big_integer r;
    uhalf memory_error;
    int n_bit_count;
    int word_size = sizeof(n.limb[0]) * 8;

    big_integer_from_u32(error, &q, 0);
    q.length = n.length;
    n_bit_count = 32 - memory_count_leading_zeros(&memory_error, n.limb[n.length - 1]);
    n_bit_count += n.length * word_size;

    for(i = n_bit_count - 1; i >= 0; i--)
    {
        int bit;
        big_integer_shift_left(error, &r, 1);
        bit = 0; /**/
        r.limb[0] = r.limb[0] | bit;
        /**/
    }

    return;
}

INTERNAL bool ieee754_error_is_set(uhalf *error)
{
    return (!error) ? (0) : (*error != IEEE754_ERROR_SUCCESS);
}

INTERNAL void ieee754_error_set(uhalf *error, uhalf error_code)
{
    if(error)
    {
        *error = error_code;
    }
}

INTERNAL void ieee754_error_get_message(uhalf *error, u32 error_code, char *buffer, u32 buffer_size)
{
    char *string = "?";
    u32 count = 0;
    uhalf string_error = STRING_ERROR;

    if(ieee754_error_is_set(error))
    {
        return;
    }

    switch(error_code)
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
            ieee754_error_set(error, IEEE754_ERROR);
        } break;
    }

    string_copy(&string_error, &count, buffer, buffer_size, string, string_length(&string_error, string));
    ieee754_error_set(error, BIG_INTEGER_ERROR_SUCCESS);

    return;
}

INTERNAL bool ieee754_double_is_infinity(uhalf *error, bool *sign, f64 value)
{
    bool result = 0;
    ieee754_double u = {0};
    u32 low = 0;
    u32 high = 0;
    u32 exponent = 0;

    if(ieee754_error_is_set(error))
    {
        return 0;
    }
    
    u.d = value;

    if(memory_is_little_endian())
    {
        high = u.i[1];
        *sign = (high >> 31) & 1;
        exponent = (high >> 20) & 0x7FF;
        high &= 0xFFFFF;
        low = u.i[0];

        result = (exponent == 0x7FF) && !low && !high;
    }
    else
    {
        ieee754_error_set(error, IEEE754_ERROR_ENDIANESS_NOT_SUPPORTED);
        return 0;
    }

    ieee754_error_set(error, IEEE754_ERROR_SUCCESS);

    return result;
}

INTERNAL bool ieee754_double_is_not_a_number(uhalf *error, f64 value)
{
    bool result = 0;
    ieee754_double u = {0};
    u32 low = 0;
    u32 high = 0;
    u32 exponent = 0;

    if(ieee754_error_is_set(error))
    {
        return 0;
    }
    
    u.d = value;

    if(memory_is_little_endian())
    {
        high = u.i[1];
        exponent = (high >> 20) & 0x7FF;
        high &= 0xFFFFF;
        low = u.i[0];

        result = (exponent == 0x7FF) && (low != 0) && (high != 0);
    }
    else
    {
        ieee754_error_set(error, IEEE754_ERROR_ENDIANESS_NOT_SUPPORTED);
        return 0;
    }

    ieee754_error_set(error, IEEE754_ERROR_SUCCESS);

    return result;
}

INTERNAL void ieee754_decode_double(uhalf *error, f64 value, bool *sign, int *exponent2, u64_emulated *mantissa)
{
    ieee754_double u = {0};
    u32 low = 0;
    u32 high = 0;
    int biased_exponent = 0;

    if(ieee754_error_is_set(error))
    {
        return;
    }

    u.d = value;

    if(memory_is_little_endian())
    {    
        low = u.i[0];
        high = u.i[1];
    }
    else
    {
        ieee754_error_set(error, IEEE754_ERROR_ENDIANESS_NOT_SUPPORTED);
        return;
    }

    *sign = (high >> 31) & 1;
    biased_exponent = (high >> 20) & 0x7FF;
    mantissa->low = low & MAX_U32;
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

    ieee754_error_set(error, IEEE754_ERROR_SUCCESS);

    return;
}

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

STRING_API void string_copy_float64(uhalf *error, u32 *count, char *buffer, u32 buffer_size, f64 value, int precision)
{
    if(string_error_is_set(error))
    {
        return;
    }

#if 1
    u32 result = STRING_ERROR;
    char work_buffer[1024];
    u32 work_buffer_length = 0;

    string_float64_backwards(error, work_buffer, sizeof(work_buffer), value, precision, &work_buffer_length);
    string_copy(error, count, buffer, buffer_size, work_buffer, work_buffer_length);
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

    if(memory_is_little_endian())
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