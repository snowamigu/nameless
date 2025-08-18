#define BIG_INTEGER_EXPORT
#include "big_integer.h"
#include "memory.h"
#include "ieee754.h"

#define BIG_INTEGER_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define BIG_INTEGER_MAX(a, b) (((a) > (b)) ? (a) : (b))

INTERNAL uhalf big_integer_error_from_memory_error(uhalf memory_error)
{
    uhalf error;

    switch(memory_error)
    {
        case MEMORY_ERROR_INVALID_PARAMETER:
        {
            error = BIG_INTEGER_ERROR_INVALID_PARAMETER;
        } break;

        default:
        {
            ASSERT(!"");
            error = BIG_INTEGER_ERROR;
        } break;
    }

    return error;
}

INTERNAL uhalf big_integer_error_from_ieee754_error(uhalf ieee754_error)
{
    uhalf error;

    switch(ieee754_error)
    {
        case IEEE754_ERROR_ENDIANESS_NOT_SUPPORTED:
        {
            error = BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
        } break;

        default:
        {
            ASSERT(!"");
            error = BIG_INTEGER_ERROR;
        } break;
    }

    return error;
}

BIG_INTEGER_API uhalf big_integer_add_u32(big_integer *augend, u32 addend)
{
    u32 carry;
    int i;

    carry = addend;

    if(memory_is_little_endian())
    {
        for(i = 0; (carry != 0) && (i < augend->length); i++)
        {
            u32 sum;
            int carry2;

            sum = augend->limb[i] + addend;
            carry2 = (sum < augend->limb[i]) ? (1) : (0);
            sum += carry;
            carry2 += (sum < carry) ? (1) : (0);
            augend->limb[i] = sum;
            carry = (carry2 != 0) ? (1) : (0);
        }

        if(carry != 0)
        {
            if(augend->length < BIG_INTEGER_MAX_LIMB_COUNT)
            {
                augend->limb[augend->length++] = carry;
            }
            else
            {
                return BIG_INTEGER_ERROR_BUFFER_OVERRUN;
            }
        }
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

BIG_INTEGER_API big_integer_add(big_integer *augend, big_integer *addend, big_integer *sum)
{
    for(i = 0; i < BIG_INTEGER_MAX(augend->length, addend->length); i++)
    {
        
    }
}

BIG_INTEGER_API uhalf big_integer_is_zero(bool *is_zero, big_integer *integer_big)
{
    if(memory_is_little_endian())
    {
        *is_zero = (integer_big->length == 1) && integer_big->limb[0];
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

BIG_INTEGER_API uhalf big_integer_shift_left(big_integer *value, int times)
{
    uhalf error;
    int word_size;
    int limbs;
    int bits;
    int i;
    u32 carry;

    if(memory_is_little_endian())
    {
        int carry2;

        word_size = sizeof(value->limb[0]) * 8;
        limbs = times / word_size;
        bits = times % word_size;

        for(i = value->length - 1; i >= 0; i--)
        {
            value->limb[i + limbs] = value->limb[i];
        }

        for(i = 0; i < limbs; i++)
        {
            value->limb[i] = 0;
        }

        value->length += limbs;

        if(value->length >= BIG_INTEGER_MAX_LIMB_COUNT)
        {
            return BIG_INTEGER_ERROR_BUFFER_OVERRUN;
        }

        error = big_integer_is_zero(&carry, value);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        carry = 0;

        for(i = 0; i < value->length; i++)
        {   
            u32 limb = value->limb[i];
            u32 shift_value;

            shift_value = limb << bits;
            shift_value &= MAX_U32;

            value->limb[i] = shift_value + carry;
            value->limb[i] &= MAX_U32;

            carry = shift_value < limb;
            carry &= MAX_U32;
        }

        if(carry != 0)
        {
            if(value->length < BIG_INTEGER_MAX_LIMB_COUNT)
            {
                value->limb[value->length++] = carry;
            }
            else
            {
                return BIG_INTEGER_ERROR_BUFFER_OVERRUN;
            }
        }
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

BIG_INTEGER_API uhalf big_integer_zero(big_integer *value)
{
    uhalf error;

    value->length = 0;
    error = memory_zeroed(value->limb, sizeof(value->limb));

    if(error != MEMORY_ERROR_SUCCESS)
    {
        return big_integer_error_from_memory_error(error);
    }

    value->length = 1;

    return BIG_INTEGER_ERROR_SUCCESS;
}

BIG_INTEGER_API uhalf big_integer_shift_right(big_integer *value, int times)
{
    int word_size;
    int limbs;
    int bits;
    int i;
    u32 carry;

    if(times < 1)
    {
        return BIG_INTEGER_ERROR_INVALID_PARAMETER;
    }

    if(memory_is_little_endian())
    {
        word_size = sizeof(value->limb[0]) * 8;
        limbs = times / word_size;
        bits = times % word_size;

        for(i = 0; i < value->length; i++)
        {
            value->limb[i] = value->limb[i + limbs];
        }

        value->length -= limbs;

        if(value->length < 0)
        {
            return BIG_INTEGER_ERROR_BUFFER_UNDERRUN;
        }

        carry = 0;
        
        for(i = 0; (bits > 0) && (i < value->length); i++)
        {   
            u32 shift_value;

            shift_value = value->limb[i] >> bits;
            shift_value &= MAX_U32;

            value->limb[i] = shift_value + carry;
            value->limb[i] &= MAX_U32;

            carry = value->limb[i] << (word_size - bits);
            carry &= MAX_U32;
        }

        if(carry != 0)
        {
            if(value->length < BIG_INTEGER_MAX_LIMB_COUNT)
            {
                value->limb[value->length++] = carry;
            }
            else
            {
                return BIG_INTEGER_ERROR_BUFFER_OVERRUN;
            }
        }
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

BIG_INTEGER_API uhalf big_integer_from_u32(big_integer *integer_big, u32 value)
{
    uhalf error;

    if(memory_is_little_endian())
    {
        integer_big->length = 0;
        error = memory_zeroed(integer_big->limb, sizeof(integer_big->limb));

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        integer_big->limb[integer_big->length++] = value;
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

BIG_INTEGER_API uhalf big_integer_from_u64_emulated(big_integer *integer_big, u64_emulated value)
{
    uhalf error;

    if(memory_is_little_endian())
    {
        integer_big->length = 0;
        error = memory_zeroed(integer_big->limb, sizeof(integer_big->limb));

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        integer_big->limb[integer_big->length++] = value.low;

        if(value.high > 0)
        {
            integer_big->limb[integer_big->length++] = value.high;   
        }
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

BIG_INTEGER_API uhalf big_integer_multiply_u32(big_integer *integer_big, u32 value)
{
    int i;
    u32 carry;

    if(memory_is_little_endian())
    {
        carry = 0;

        for(i = 0; i < integer_big->length; i++)
        {
            u32 multiplicand;
            u32 multiplier;
            u32 carry2;

            multiplicand = integer_big->limb[i];
            multiplier = value;

            integer_big->limb[i] = multiplicand * multiplier;
            carry2 = (integer_big->limb[i] < BIG_INTEGER_MAX(multiplicand, multiplier)) ? (1) : (0);
            
            integer_big->limb[i] += carry;
            carry2 += (integer_big->limb[i] < carry) ? (1) : (0);
            
            carry = (carry2 != 0) ? (1) : (0);
        }
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

BIG_INTEGER_API uhalf big_integer_copy(big_integer *source, big_integer *destination)
{
    uhalf error;

    destination->length = 0;

    error = memory_zeroed(destination->limb, sizeof(destination->limb));

    if(error != MEMORY_ERROR_SUCCESS)
    {
        return big_integer_error_from_memory_error(error);
    }
    
    if(memory_is_little_endian())
    {
        int i;

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

BIG_INTEGER_API uhalf big_integer_from_double(f64 value, big_integer *numerator, big_integer *denominator)
{
    /* (-1)^sign * 0.fraction * 2^(E - bias)*/
    /* (-1)^sign * 1.fraction * 2^(E - bias)*/

    /*
        Normalized number = ((2^52 + F) / 2^52) * 2^(E - bias):
        k = min(52, E - bias)
        numerator = (2^52 + F) * 2^(E - bias - k)
        denominator = 2^(52 - k)
        
        Denormalized number = (F * 2^(1 - bias)) / 2^52:
        F = f_odd * 2^f2. f2 == number of traling zeros in F
        ((f_odd * 2^f2) * 2^(1 - bias)) / 2^52
        (f_odd * 2^(f2 + 1 - bias)) / 2^52
        k = min(52, f2 + 1 - bias)
        numerator = f_odd * 2^(f2 + 1 - bias - k)
        denominator = 2^(52 - k)
    */
    
    uhalf error;
    ieee754_double ieee754;
    big_integer fraction;
    int word_size;
    int bias;

    word_size = sizeof(numerator->limb[0]) * 8;
    bias = 1023;

    error = ieee754_from_double(value, &ieee754);

    if(error != IEEE754_ERROR_SUCCESS)
    {
        return big_integer_error_from_ieee754_error(error);
    }

    error = big_integer_zero(numerator);

    if(error != BIG_INTEGER_ERROR_SUCCESS)
    {
        return error;
    }

    error = big_integer_zero(numerator);

    if(error != BIG_INTEGER_ERROR_SUCCESS)
    {
        return error;
    }

    if(ieee754.is_subnormal)
    {
        /*
            F = f_odd * 2^f2. f2 == number of traling zeros in F
            numerator = f_odd * 2^(f2 + 1 - bias - k)
            numerator = f_odd
            denominator = 2^(52 - k)
        */

        big_integer fraction_odd;
        int f2;
        int k;
        f2 = memory_count_trailing_zeros_u64_emulated(ieee754.fraction);
        k = BIG_INTEGER_MIN(52, f2 + 1 - bias);
        k = (k < 0) ? (0) : (k);

        error = big_integer_from_u64_emulated(&fraction_odd, ieee754.fraction);
        
        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        if(f2 > 0)
        {
            error = big_integer_shift_right(&fraction_odd, f2);

            if(error != BIG_INTEGER_ERROR_SUCCESS)
            {
                return error;
            }
        }

        error = big_integer_copy(&fraction_odd, numerator);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        error = big_integer_from_u32(denominator, 1);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        error = big_integer_shift_left(denominator, bias + 51 - f2);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }
    }
    else
    {
        ASSERT(0);
    }

        return BIG_INTEGER_ERROR_SUCCESS;
}

BIG_INTEGER_API uhalf big_integer_normalize(big_integer *integer_big)
{
    while((integer_big->length > 1) && !integer_big->limb[integer_big->length - 1])
    {
        integer_big->length--;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

BIG_INTEGER_API uhalf big_integer_divide_u32(big_integer *integer_big, u32 value, big_integer *quotient, u32 *remainder)
{
    uhalf error;
    int i;
    big_integer q;
    u32 r;

    if(!value)
    {
        return BIG_INTEGER_ERROR_DIVISION_BY_ZERO;
    }

    if(memory_is_little_endian())
    {
        q.length = integer_big->length;

        for(i = integer_big->length - 1; i >= 0; i--)
        {
            u32 dividend;
            u32 divisor;

            dividend = integer_big->limb[i];
            divisor = value;

            q.limb[i] = dividend / divisor;
            r += dividend % divisor;
        }

        error = big_integer_normalize(&q);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        error = big_integer_copy(&q, quotient);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        *remainder = r;
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

INTERNAL void add_u32_u32(u32 augend, u32 addend, u64_emulated *result)
{
    u32 carry;
    
    result->low = augend + addend;
    carry = (augend < addend) ? (result->low) : (0);
    result->high = carry;
}

INTERNAL void multiply_u32_u32(u32 multiplicand, u32 multiplier, u64_emulated *result)
{
    u32 carry;
    
    result->low = multiplicand * multiplier;
    carry = (result->low < BIG_INTEGER_MIN(multiplicand, multiplier)) ? (result->low) : (0);
    result->high = carry;
}

INTERNAL void add_u64_emulated(u64_emulated augend, u64_emulated addend, u128_emulated *result)
{
    /*
        u128 = ((a_high << 64) + a_low) + ((b_high << 64) + b_low)
        (a_high << 64) + (b_high << 64) + a_low + b_low
    */

    u64_emulated p0 = {0};
    u64_emulated p1 = {0};
    u32 carry;
    u64_emulated temporary = {0};

    add_u32_u32(augend.low, addend.low, &p0);
    carry = (p0.high > 0) ? (1) : (0);
    add_u32_u32(augend.high, addend.high, &p1);

    if(carry != 0)
    {
        add_u32_u32(p1.low, carry, &temporary);
    }
    
    result->low = p0;
    result->high = temporary;
}

INTERNAL void multiply_u64_emulated(u64_emulated multiplicand, u64_emulated multiplier, u128_emulated *result)
{
    /*
        u128 = ((a_high << 64) + a_low) * ((b_high << 64) + b_low)
        ((a_high * b_high) << 128) + ((a_high * b_low) << 64) + ((a_low * b_high) << 64) + ((a_low * b_low))

    */
    u64_emulated p0 = {0};
    u64_emulated p1 = {0};
    u64_emulated p2 = {0};
    u64_emulated p3 = {0};
    u64_emulated carry = {0};

    multiply_u32_u32(multiplicand.low, multiplier.low, &p0);
    multiply_u32_u32(multiplicand.low, multiplier.high, &p1);
    multiply_u32_u32(multiplicand.high, multiplier.low, &p2);
    multiply_u32_u32(multiplicand.high, multiplier.high, &p3);

    /**/
}

BIG_INTEGER_API uhalf big_integer_divide(big_integer *numerator, big_integer *denominator, big_integer *quotient, big_integer *remainder)
{
    uhalf error;

    if(memory_is_little_endian())
    {
        int denominator_limbs_shift;
        int denominator_bits_shift;
        big_integer num;
        big_integer dem;
        big_integer quo;
        big_integer rem;
        int i;
        int m;
        int n;

        denominator_bits_shift = memory_count_leading_zeros_u32(denominator->limb[denominator->length - 1]);

        error = big_integer_copy(numerator, &num);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        error = big_integer_copy(denominator, &dem);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        error = big_integer_shift_left(&dem, denominator_bits_shift);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        error = big_integer_shift_left(&num, denominator_bits_shift);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        n = numerator->length;
        m = denominator->length;
        big_integer_copy(numerator, &rem);

        for(i = n - m; i >= 0; i--)
        {
            u32 n_limb1;
            u32 n_limb2;
            u32 divisor;
            u64_emulated q_guess;
            u64_emulated r_guess;
            
            n_limb1 = num.limb[i];
            n_limb2 = num.limb[i - 1];
            q_guess;
        }

        error = big_integer_shift_right(&num, denominator_bits_shift);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        error = big_integer_shift_right(&dem, denominator_bits_shift);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        error = big_integer_copy(&num, numerator);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }

        error = big_integer_copy(&dem, denominator);

        if(error != BIG_INTEGER_ERROR_SUCCESS)
        {
            return error;
        }
    }
    else
    {
        return BIG_INTEGER_ERROR_ENDIANESS_NOT_SUPPORTED;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}

BIG_INTEGER_API uhalf big_integer_to_integer_fraction_parts(big_integer *numerator, int *integer_part, int integer_part_count, big_integer *denominator, int *denominator_part, int denominator_part_count)
{
    uhalf error;

    big_integer i;
    big_integer f;
    big_integer q;
    big_integer r;

    error = big_integer_divide(numerator, denominator, &q, &r);

    if(error != BIG_INTEGER_ERROR_SUCCESS)
    {
        return error;
    }

    error = big_integer_copy(&q, &numerator);

    if(error != BIG_INTEGER_ERROR_SUCCESS)
    {
        return error;
    }

    return BIG_INTEGER_ERROR_SUCCESS;
}