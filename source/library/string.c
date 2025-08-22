#define STRING_EXPORT
#include "string.h"

typedef union
{
    f64 d;
    u32 i[2];
} string_ieee754_double_union;

typedef struct
{
    bool sign;
    int exponent;
    u64_emulated fraction;
} string_ieee754_double;

#define STRING_BIG_INTEGER_MAX_LIMB_COUNT 2048

typedef struct
{
    bool sign;
    u32 limb[STRING_BIG_INTEGER_MAX_LIMB_COUNT];
    int length;
} string_big_integer;

INTERNAL bool string_memory_is_little_endian()
{
    unsigned int i = 1;
    unsigned char *p = (unsigned char *)&i;

    return *p == 1;
}

INTERNAL void string_memory_zeroed(void *memory, u32 size)
{
    u8 *m;
    u32 i;

    m = memory;

    for(i = 0; i < size; i++)
    {
        *m = 0;
    }
}

INTERNAL int string_memory_is_bit_set_u32(u32 value, int index)
{
    return value & (1 << index);
}

INTERNAL int string_memory_count_trailing_bits_u32(u32 value, int bit)
{
    int count;
    int i;

    count = 0;

    for(i = 0; i < 32; i++)
    {
        if(string_memory_is_bit_set_u32(value, i) == bit)
        {
            count++;
        }
        else
        {
            break;
        }
    }

    return count;
}

INTERNAL int string_memory_count_leading_bits_u32(u32 value, int bit)
{
    int count;
    int i;

    count = 0;

    for(i = 31; i >= 0; i--)
    {
        if(string_memory_is_bit_set_u32(value, i) == bit)
        {
            count++;
        }
        else
        {
            break;
        }
    }

    return count;
}

INTERNAL void string_u32_multiply(u32 left_operand, u32 right_operand, u64_emulated *product)
{
    /*
        ((ah << 16) + al) * ((bh << 16) + bl)
        ((ah * bh) << 32) + ((ah *bl) << 16) + ((al * bh) << 16) + (al * bl)
        v = p3 + p2 + p1 + p0
        carry = 0
        low = p0 + (p1 << 16) + (p2 << 16) + carry
        carry = p0 > 0xFFFFFFFF - (p1 >> 16)
        carry += carry > 0xFFFFFFFF - (p2 >> 16)
        high = p3 + (p2 >> 16) + (p1 >> 16) + carry
    */

    u32 carry;
    int i;
    u32 al;
    u32 ah;
    u32 bl;
    u32 bh;
    u32 p0;
    u32 p1;
    u32 p2;
    u32 p3;
    u32 low;
    u32 high;

    carry = 0;
    
    al = left_operand & MAX_U16;
    ah = (left_operand >> 16) & MAX_U16;
    bl = right_operand & MAX_U16;
    bh = (right_operand >> 16) & MAX_U16;
    
    p0 = al * bl;
    p1 = al * bh;
    p2 = ah * bl;
    p3 = ah * bh;

    low = p0 + ((p1 << 16) & MAX_U32);
    carry = (low > (MAX_U32 - ((p1 << 16) & MAX_U32))) ? (1) : (0); /* here! */
    low += ((p2 << 16) & MAX_U32);
    carry += (low > (MAX_U32 - ((p2 << 16) & MAX_U32))) ? (1) : (0);

    high = p3 + (p2 >> 16) + (p1 >> 16) + carry;

    product->low = low;
    product->high = high;
}

INTERNAL void string_ieee754_from_double(f64 value, string_ieee754_double *ieee754)
{
    string_ieee754_double_union u = {0};

    u.d = value;

    string_memory_zeroed(ieee754, sizeof(string_ieee754_double));

    ieee754->sign = (u.i[1] >> 31) & MAX_U32;
    ieee754->exponent = (u.i[1] >> 20) & 0x7FF;
    ieee754->fraction.low = u.i[0] & MAX_U32;
    ieee754->fraction.high = u.i[1] & 0xFFFFF;
}

INTERNAL bool string_ieee754_is_zero(int exponent, u64_emulated fraction)
{
    return !exponent && !fraction.low && !fraction.high;
}

INTERNAL bool string_ieee754_is_infinity(int exponent, u64_emulated fraction)
{
    return (exponent == 0x7FF) && !fraction.low && !fraction.high;
}

INTERNAL bool string_ieee754_is_not_a_number(int exponent, u64_emulated fraction)
{
    return (exponent == 0x7FF) && ((fraction.low != 0) || (fraction.high != 0));
}

INTERNAL void string_big_integer_zeroed(string_big_integer *value)
{
    value->sign = value->length = 0;
    string_memory_zeroed(value, sizeof(string_big_integer));
}

INTERNAL void string_big_integer_normalize(string_big_integer *value)
{
    while((value->length > 1) && !value->limb[value->length - 1])
    {
        value->length--;
    }
}

INTERNAL void string_big_integer_copy(string_big_integer *source, string_big_integer *destination)
{
    int i;

    if(string_memory_is_little_endian())
    {
        destination->sign = destination->length = 0;
        string_big_integer_zeroed(destination);

        for(i = 0; i < source->length; i++)
        {
            destination->limb[i] = source->limb[i];
        }

        destination->length = source->length;
    }
    else
    {
        return;
    }
}

INTERNAL void string_big_integer_from_u32(u32 value, string_big_integer *result)
{
    if(string_memory_is_little_endian())
    {
        string_big_integer_zeroed(result);
        result->limb[result->length++] = value;
    }
    else
    {
        return;
    }
}

INTERNAL void string_big_integer_from_u64_emulated(u64_emulated *value, string_big_integer *result)
{
    if(string_memory_is_little_endian())
    {
        string_big_integer_zeroed(result);
        
        result->limb[result->length++] = value->low;
        
        if(value->high != 0)
        {
            result->limb[result->length++] = value->high;
        }
    }
    else
    {
        return;
    }
}

INTERNAL int string_big_integer_compare(string_big_integer *left_operand, string_big_integer *right_operand)
{
    int i;

    if(left_operand->length != right_operand->length)
    {
        return (left_operand->length > right_operand->length) ? (1) : (-1);
    }
    
    if(left_operand->length == right_operand->length)
    {
        for(i = 0; i < left_operand->length; i++)
        {
            if(left_operand->limb[i] > right_operand->limb[i])
            {
                return 1;
            }
            else if(left_operand->limb[i] < right_operand->limb[i])
            {
                return -1;
            }
        }
    }

    return 0;
}

INTERNAL void string_big_integer_euclidean_algortihm()
{
}

INTERNAL void string_big_integer_add(string_big_integer *left_operand, string_big_integer *right_operand, string_big_integer *result)
{
    int i;
    string_big_integer r;
    int max_length;
    int carry;

    max_length = MAX(left_operand->length, right_operand->length);
    carry = 0;
    r.length = left_operand->length + right_operand->length - 1;

    for(i = 0; i < max_length; i++)
    {
        int left;
        int right;
        int carry2;

        left = (i < left_operand->length) ? (left_operand->limb[i]) : (0);
        right = (i < right_operand->length) ? (right_operand->limb[i]) : (0);

        r.limb[i] = left + right;
        carry2 = (r.limb[i] > MAX_U32 - right) ? (1) : (0);
        r.limb[i] += carry;
        carry2 += (r.limb[i] > MAX_U32 - carry) ? (1) : (0);
        carry = (carry2 != 0) ? (carry2) : (0);
    }

    if(carry != 0)
    {
        r.limb[r.length++] = carry;
    }

    string_big_integer_normalize(&r);

    string_big_integer_copy(&r, result);
}

INTERNAL void string_big_integer_shift_left(string_big_integer *left_operand, int times, string_big_integer *result)
{
    int i;
    int word_size;
    int limbs;
    int bits;
    u32 carry;
    string_big_integer r;

    if(string_memory_is_little_endian())
    {
        word_size = sizeof(left_operand->limb[0]) * 8;
        limbs = times / word_size;
        bits = times % word_size;

        string_big_integer_copy(left_operand, &r);

        for(i = 0; (limbs != 0) && (i < limbs); i++)
        {
            r.limb[i + limbs] = left_operand->limb[i];
        }

        for(i = 0; (limbs != 0) && (i < left_operand->length); i++)
        {
            r.limb[i] = 0;
        }

        r.length = left_operand->length + limbs;
        
        carry = 0;

        for(i = 0; (bits != 0) && (i < r.length); i++)
        {
            u32 limb;

            limb = (r.limb[i] << bits) & MAX_U32;
            r.limb[i] = limb | carry;
            carry = (r.limb[i] >> (word_size - bits)) & MAX_U32;
        }

        string_big_integer_copy(&r, result);
    }
    else
    {
        return;
    }
}

INTERNAL void string_big_integer_shift_right(string_big_integer *left_operand, int times, string_big_integer *result)
{
    int i;
    int word_size;
    int limbs;
    int bits;
    u32 carry;

    if(string_memory_is_little_endian())
    {
        word_size = sizeof(left_operand->limb[0]) * 8;
        limbs = times / word_size;
        bits = times % word_size;

        for(i = limbs; (limbs > 0) && (i < left_operand->length); i++)
        {
            left_operand->limb[i - limbs] = left_operand->limb[i];
        }

        left_operand->length -= limbs;

        carry = 0;

        for(i = 0; (bits != 0) && (i < left_operand->length); i++)
        {
            u32 limb;

            limb = (left_operand->limb[i] >> bits) & MAX_U32;
            result->limb[i] = limb | carry;
            carry = (left_operand->limb[i] << (word_size - bits)) & MAX_U32;
        }
    }
    else
    {
        return;
    }
}

INTERNAL void string_big_integer_multiply(string_big_integer *left_operand, string_big_integer *right_operand, string_big_integer *result)
{
    int i;
    string_big_integer r;
    u32 carry;

    carry = 0;
    r.length = left_operand->length + right_operand->length;

    for(i = 0; i < left_operand->length; i++)
    {
        u32 multiplier;
        u32 multiplicand;
        u64_emulated product = {0};
        int j;

        multiplier = (i < left_operand->length) ? (left_operand->limb[i]) : (0);

        for(j = i; j < right_operand->length; j++)
        {
            u32 carry2;

            multiplicand = right_operand->limb[j];
            string_u32_multiply(multiplier, multiplicand, &product);

            r.limb[j] = product.low + carry;
            carry = (product.low > (MAX_U32 - carry)) ? (1) : (0);
            carry += product.high;
        }
#if 0
        u32 multiplier;
        u32 multiplicand;
        u64_emulated product = {0};
        int j;

        multiplier = (i < left_operand->length) ? (left_operand->limb[i]) : (0);

        for(j = 0; j < right_operand->length; j++)
        {
            u32 carry2;

            multiplicand = (j < right_operand->length) ? (right_operand->limb[j]) : (0);
            string_u32_multiply(multiplier, multiplicand, &product);

            r.limb[i] = product.low + carry;
            carry = (product.low > (MAX_U32 - carry)) ? (1) : (0);
            carry += product.high;
        }
#endif
    }

    string_big_integer_normalize(&r);

    if(carry != 0)
    {
        r.limb[r.length++] = carry;
    }

    string_big_integer_copy(&r, result);
}

INTERNAL bool string_big_integer_is_zero(string_big_integer *value)
{
    if(string_memory_is_little_endian())
    {
        return (value->length == 1) && !value->limb[0];
    }
    else
    {
        return 0;
    }
}

INTERNAL void string_big_integer_from_ieee754(bool sign, int exponent, int bias, u64_emulated *fraction, string_big_integer *numerator, string_big_integer *denominator)
{
    bool is_subnormal;
    string_big_integer n;
    string_big_integer d;
    string_big_integer temporary;
    string_big_integer temporary2;
    int e;

    is_subnormal = !exponent;

    if(is_subnormal)
    {
        /*
            e = -1022
            (-1)^s * 2^e * 0.f
            (-1)^s * 2^-1022 * f/2^52
            (-1)^s * 2^-1074 * f
            (-1)^s * f/2^1074

        */

        string_big_integer_from_u64_emulated(fraction, &n);
        string_big_integer_shift_left(&n, 1074, &n);

        string_big_integer_from_u32(1, &d);

        string_big_integer_copy(&n, numerator);
        string_big_integer_copy(&d, denominator);
    }
    else
    {
        /*
            0 < e < 2047
            (-1)^s * 2^(e - 1023) * 1.f
            (-1)^s * 2^(e - 1023) * (2^52 + f)/2^52
        */

        e = exponent - bias;

        if(e >= 0)
        {
            string_big_integer_from_u32(1, numerator);
            string_big_integer_shift_left(numerator, e, numerator);

            string_big_integer_from_u32(1, &temporary);
            string_big_integer_shift_left(&temporary, 52, &temporary);

            string_big_integer_from_u64_emulated(fraction, &temporary2);

            string_big_integer_add(&temporary, &temporary2, &temporary);

            string_big_integer_multiply(numerator, &temporary, numerator);

            string_big_integer_from_u32(1, denominator);
            string_big_integer_shift_left(denominator, 52, denominator);
        }
        else
        {
            ASSERT(0);

            string_big_integer_from_u32(1, numerator);
            string_big_integer_shift_left(numerator, exponent, numerator);

            string_big_integer_from_u64_emulated(fraction, &temporary);

            string_big_integer_multiply(numerator, &temporary, numerator);
            
            string_big_integer_shift_left(numerator, 1075, numerator);

            string_big_integer_from_u32(1, denominator);
        }
    }
}

INTERNAL void string_big_integer_divide(string_big_integer *numerator, string_big_integer *denominator, string_big_integer *quotient, string_big_integer *remainder)
{
    u32 carry;
    int i;
    string_big_integer n;
    string_big_integer d;
    string_big_integer q;
    string_big_integer r;
    int shift_n;
    int shift_d;
    int shift;
    int word_size;
    int k;
    int j;

    if(string_memory_is_little_endian())
    {
        carry = 0;
        word_size = sizeof(numerator->limb[0]) * 8;

        if(string_big_integer_compare(numerator, denominator) < 0)
        {
            string_big_integer_from_u32(0, quotient);
            string_big_integer_copy(numerator, remainder);
            return;
        }

        string_big_integer_copy(numerator, &n);
        string_big_integer_copy(denominator, &d);
        string_big_integer_from_u32(0, &q);
        string_big_integer_from_u32(0, &r);

        shift_n = 0;
        shift_d = 0;

        if(d.length > n.length)
        {
            shift_n += (d.length > n.length) ? ((d.length - n.length) * word_size) : (0);
            
            if(shift_n > 0)
            {
                string_big_integer_shift_left(&n, shift_n, &n);
            }
        }
        else if(n.length > d.length)
        {
            shift_d += (n.length > d.length) ? ((n.length - d.length) * word_size) : (0);
            
            if(shift_d > 0)
            {
                string_big_integer_shift_left(&d, shift_d, &d);
            }
        }
#if 0
        shift = string_memory_count_leading_bits_u32(n.limb[n.length - 1], 0);
        if(shift > 0)
        {
            string_big_integer_shift_left(&n, shift, &n);
            shift_n += shift;
        }

        shift = string_memory_count_leading_bits_u32(d.limb[d.length - 1], 0);
        if(shift > 0)
        {
            string_big_integer_shift_left(&d, shift, &d);
            shift_d += shift;
        }
#endif
        q.length = numerator->length - denominator->length + 1;
        r.length = numerator->length - denominator->length + 1;
        j = q.length;
        k = r.length;

        /*
            a = bq + r
            0 <= r < d
            a = bq + r
            q = (a - (b - 1)) / b
            q = a / b

            a = bq + r
            a - r = bq
            (a - r) / q = b
        */
        
        for(i = n.length - 1; (j + k) > 0; i--)
        {
            u64_emulated quotient_guess = {0};
            u32 running_count;
            u32 numerator_digit;
            u32 denominator_digit;

            numerator_digit = n.limb[i];
            denominator_digit = d.limb[i];
            running_count = (!denominator_digit) ? (0) : (numerator_digit / denominator_digit);

            if(!denominator_digit)
            {
                quotient_guess.low = 0;
            }
            else
            {
                while(quotient_guess.low < numerator_digit)
                {
                    string_u32_multiply(running_count, denominator_digit, &quotient_guess);
                    
                    if(quotient_guess.high != 0)
                    {
                        break;
                    }
                    else if(quotient_guess.low < numerator_digit)
                    {
                        running_count++;
                    }
                }

                if((quotient_guess.high != 0) || (numerator_digit < quotient_guess.low))
                {
                    quotient_guess.low = running_count - 1;
                }
                else
                {
                    quotient_guess.low = running_count;
                }
            }

            q.limb[--j] =  quotient_guess.low;
            r.limb[--k] = numerator_digit - (denominator_digit * quotient_guess.low);
        }

        if(shift_n > 0)
        {
            string_big_integer_shift_right(&q, shift_n, &q);
            string_big_integer_shift_right(&r, shift_n, &r);
            //string_big_integer_shift_right(&q, shift, &q);
            //string_big_integer_shift_right(&r, shift, &r);
        }
        else if(shift_d > 0)
        {
            string_big_integer_shift_right(&q, shift_d, &q);
            string_big_integer_shift_right(&r, shift_d, &r);
            //string_big_integer_shift_right(&q, shift, &q);
            //string_big_integer_shift_right(&r, shift, &r);
        }

        string_big_integer_normalize(&q);
        string_big_integer_normalize(&r);
        
        string_big_integer_copy(&q, quotient);
        string_big_integer_copy(&r, remainder);
    }
    else
    {
        return;
    }
}

INTERNAL void string_big_integer_round_half_to_even(string_big_integer *value, int precision)
{
    int i;
    int j;

    for(i = 0; i < value->length; i++)
    {
        
    }

}

INTERNAL u32 string_big_integer_to_string(string_big_integer *value, char *buffer)
{
    u32 count;
    int i;
    int list[324];
    int list_index;

    count = 0;
    list_index = 0;

    if(string_memory_is_little_endian())
    {
        if((value->length == 1) && !value->limb[0])
        {
            *buffer = '0';
            count++;

            return count;
        }

        for(i = 0; i < value->length; i++)
        {
            u32 limb;

            limb = value->limb[i];

            while(limb)
            {
                list[list_index++] = limb % 10;
                limb /= 10;
            }
        }

        while(--list_index >= 0)
        {
            *buffer++ = (char)list[list_index] + '0';
            count++;
        }
    }
    else
    {
        return 0;
    }

    return count;
}

INTERNAL bool string_double_handle_special_case(u32 *count, char *buffer, int precision, string_ieee754_double *ieee754)
{
    bool is_special_case = 1;

    if(string_ieee754_is_zero(ieee754->exponent, ieee754->fraction))
    {
        char *string;
        
        string = (ieee754->sign) ? ("-0.") : ("0.");

        *count += string_copy(buffer + *count, string, string_length(string));

        while(precision-- > 0)
        {
            *count += string_copy_character(buffer + *count, '0');
        }
    }
    else if(string_ieee754_is_infinity(ieee754->exponent, ieee754->fraction))
    {
        char *string;
        
        string = (ieee754->sign) ? ("-inf") : ("inf");

        *count += string_copy(buffer + *count, string, string_length(string));
    }
    else if(string_ieee754_is_not_a_number(ieee754->exponent, ieee754->fraction))
    {
        bool qnan;
        char *string;
        
        qnan = (ieee754->fraction.high >> (20 - 1)) & 1;
        string = (qnan) ? ("qNaN") : ("sNaN");

        *count += string_copy(buffer + *count, string, string_length(string));
    }
    else
    {
        is_special_case = 0;
    }

    return is_special_case;
}

STRING_API u32 string_length(char *string)
{
    u32 count = 0;

    while(*string++ != '\0')
    {
        count++;
    }

    return count;
}

STRING_API u32 string_copy_character(char *buffer, int character)
{
    *buffer = (char)character;
    
    return 1;
}

STRING_API u32 string_copy_integer(char *buffer, bool sign, u32 value, int base, char *representation)
{
    u32 count;
    char work_buffer[32 + 1] = {0};
    int work_buffer_index;

    count = 0;
    work_buffer_index = ARRAY_COUNT(work_buffer) - 2;

    if(!value)
    {
        return string_copy_character(buffer, '0');
    }

    if(sign)
    {
        count += string_copy_character(buffer + count, '-');
        value = -(s32)value;
    }

    while(value)
    {
        work_buffer[work_buffer_index--] = representation[value % base];
        value /= base;
    }
    
    work_buffer_index++;

    while(work_buffer[work_buffer_index] != '\0')
    {
        count += string_copy_character(buffer + count, work_buffer[work_buffer_index]);
        work_buffer_index++;
    }

    return count;
}

STRING_API u32 string_copy_double(char *buffer, f64 value, int precision)
{
    u32 count;
    string_big_integer numerator;
    string_big_integer denominator;
    string_big_integer integer_part;
    string_big_integer fraction_part;

    count = 0;

#if 1
    string_ieee754_double ieee754 = {0};

    string_ieee754_from_double(value, &ieee754);

    if(string_double_handle_special_case(&count, buffer + count, precision, &ieee754))
    {
        return count;
    }

    string_big_integer_from_ieee754(ieee754.sign, ieee754.exponent, 1023, &ieee754.fraction, &numerator, &denominator);

    string_big_integer_divide(&numerator, &denominator, &integer_part, &fraction_part);

    string_big_integer_round_half_to_even(&fraction_part, 16 + 1);

    if(ieee754.sign)
    {
        count += string_copy_character(buffer + count, '-');
    }

    count += string_big_integer_to_string(&integer_part, buffer + count);

    count += string_copy_character(buffer + count, '.');

    count += string_big_integer_to_string(&fraction_part, buffer + count);

#else
    s32 integer_part;
    char *decimal;
    int i;
    
    integer_part = (s32)value;
    decimal = "0123456789";
    count += string_copy_integer(buffer, value < 0, integer_part, 10, decimal);

    if(value < 0)
    {
        value = -value;
        integer_part = -integer_part;
    }

    value -= integer_part;

    count += string_copy_character(buffer + count, '.');

    for(i = 0; i < precision; i++)
    {
        value *= 10;
        integer_part = (int)value;
        count += string_copy_integer(buffer + count, 0, integer_part, 10, decimal);
        value -= integer_part;
    }
#endif

    return count;
}

STRING_API u32 string_copy_void_pointer(char *buffer, void *value, int base, char *representation)
{
    u32 count;
    u8 *byte;
    int i;

    count = 0;

    if(string_memory_is_little_endian())
    {
        byte = (u8 *)&value;

        for(i = sizeof(void *) - 1; i >= 0; i--)
        {
            if(byte[i] < 16)
            {
                count += string_copy_character(buffer + count, '0');
            }

            count += string_copy_integer(buffer + count, 0, byte[i], base, representation);
        }
    }
    else
    {
        ASSERT(0);
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

STRING_API u32 string_format(char *buffer, char *format, va_list argument_list)
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
                    count += string_copy_double(buffer + count, value, 17);
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
                case 'X':
                {
                    u32 value;
                    
                    value = va_arg(argument_list, u32);

                    if(*format == 'x')
                    {
                        count += string_copy_integer(buffer + count, 0, value, 16, hexadecimal_lowercase);
                    }
                    else
                    {
                        count += string_copy_integer(buffer + count, 0, value, 16, hexadecimal_uppercase);
                    }

                    format++;
                } break;
                
                default:
                {
                } break;
            }
        }
    }

    return count;
}