#define BIG_INTEGER_EXPORT
#include "big_integer.h"

#include "memory.h"

INTERNAL void u32_multiply(u32 multiplier, u32 multiplicand, u64_emulated *product)
{
    /*
        ((a_high << 16) + a_low) * ((b_high << 16) + b_low)
        ((a_high * b_high) << 32) + ((a_high * b_low) << 16) + ((a_low * b_high) << 16) + (a_low * b_low)
        p0 = 1, 32
        p1 = 16, 48
        p2 = 16, 48
        p3 = 32, 64
        r0 = p0 + (p1 & MAX_U16) + (p2 & MAX_U16)
        r1 = p3 + (p2 >> 16) + (p1 >> 16)
        result = {r0, r1};
    */

    int i;
    u32 a_low;
    u32 a_high;
    u32 b_low;
    u32 b_high;
    u32 p0;
    u32 p1;
    u32 p2;
    u32 p3;
    u32 p1_low;
    u32 p1_high;
    u32 p2_low;
    u32 p2_high;
    u32 low_carry;
    u32 high_carry;
    u32 old_low;
    u32 old_high;

    low_carry = 0;

    memory_zeroed(product, sizeof(u64_emulated));
    
    a_low = multiplier & MAX_U16;
    a_high = (multiplier >> 16) & MAX_U16;
    b_low = multiplicand & MAX_U16;
    b_high = (multiplicand >> 16) & MAX_U16;

    p0 = a_low * b_low;
    p1 = a_low * b_high;
    p2 = a_high * b_low;
    p3 = a_high * b_high;

    p1_low = p1 & MAX_U16;
    p1_high = (p1 >> 16) & MAX_U16;
    p2_low = p2 & MAX_U16;
    p2_high = (p2 >> 16) & MAX_U16;

    product->low = p2_low + p1_low;
    old_low = product->low;
    product->low += p0;
    high_carry = (old_low > (MAX_U32 - p0)) ? (1) : (0);
    old_low = product->low;
    product->low += low_carry;
    high_carry += (old_low > (MAX_U32 - low_carry)) ? (1) : (0);

    product->high = p3 + p2_high + p1_high;
    old_high = product->high;
    product->high += high_carry;
    low_carry = (old_high > (MAX_U32 - high_carry)) ? (1) : (0);
}

BIG_INTEGER_API void big_integer_zeroed(big_integer *value)
{
    value->sign = value->length = 0;
    
    memory_zeroed(value->limb, sizeof(value->limb));

    return;
}

BIG_INTEGER_API void big_integer_copy(big_integer *source, big_integer *destination)
{
    int i;

    big_integer_zeroed(destination);

    if(memory_is_little_endian())
    {
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

BIG_INTEGER_API void big_integer_from_u32(u32 value1, big_integer *value2)
{
    if(memory_is_little_endian())
    {
        big_integer_zeroed(value2);
        
        value2->limb[value2->length++] = value1;
    }
    else
    {
        return;
    }
}

BIG_INTEGER_API void big_integer_from_u64_emulated(u64_emulated *value1, big_integer *value2)
{
    if(memory_is_little_endian())
    {
        big_integer_zeroed(value2);
        
        value2->limb[value2->length++] = value1->low;
        value2->limb[value2->length++] = value1->high;
    }
    else
    {
        return;
    }
}

BIG_INTEGER_API void big_integer_add(big_integer *value1, big_integer *value2, big_integer *sum)
{
    int i;
    int max_length;
    big_integer r;
    u32 carry;

    max_length = MAX(value1->length, value2->length);
    carry = 0;
    
    if(memory_is_little_endian)
    {
        for(i = 0; i < max_length; i++)
        {
            u32 augend;
            u32 addend;
            u32 carry2;

            augend = (i < value1->length) ? (value1->limb[i]) : (0);
            addend = (i < value2->length) ? (value2->limb[i]) : (0);

            r.limb[i] = augend + addend;
            r.limb[i] &= MAX_U32;
            carry2 = (r.limb[i] < augend) ? (r.limb[i]) : (0);
            
            r.limb[i] += carry;
            r.limb[i] &= MAX_U32;
            carry2 += (r.limb[i] < augend) ? (1) : (0);
            
            carry = (carry2 != 0) ? (carry2) : (0);
        }

        r.length = max_length;

        if(carry != 0)
        {
            if(r.length < BIG_INTEGER_MAX_LIMBS_COUNT)
            {
                r.limb[r.length++] = carry;
            }
            else
            {
                return;
            }
        }

        big_integer_copy(&r, sum);
    }
    else
    {
        return;
    }
}

BIG_INTEGER_API void big_integer_subtract(big_integer *value1, big_integer *value2, big_integer *difference)
{
    int i;
    int max_length;
    big_integer r;
    u32 borrow;

    max_length = MAX(value1->length, value2->length);
    borrow = 0;

    if(memory_is_little_endian())
    {
        for(i = 0; i < max_length; i++)
        {
            u32 minuend;
            u32 subtrahend;
            u32 borrow2;

            minuend = (i < value1->length) ? (value1->limb[i]) : (0);
            subtrahend = (i < value2->length) ? (value2->limb[i]) : (0);

            borrow2 = (subtrahend > minuend) ? (1) : (0);
            r.limb[i] = minuend - subtrahend;
            borrow2 = (r.limb[i] < borrow) ? (1) : (borrow2);
            r.limb[i] -= borrow;
            borrow = borrow2;
        }

        r.length = value1->length - value2->length + 1;

        big_integer_copy(&r, difference);
    }
    else
    {
        return;
    }
}

BIG_INTEGER_API void big_integer_shift_left(big_integer *value1, u32 value2, big_integer *result)
{
    int word_size;
    int limbs;
    int bits;
    int i;
    big_integer r;
    u32 carry;

    word_size = sizeof(value1->limb[0]) * 8;
    limbs = value2 / word_size;
    bits = value2 % word_size;
    carry = 0;
    
    big_integer_copy(value1, &r);

    for(i = r.length - 1; i >= 0; i--)
    {
        r.limb[i + word_size] = r.limb[i];
    }

    for(i = 0; i < limbs; i++)
    {
        r.limb[i] = 0;
    }

    r.length += limbs;

    for(i = 0; i < r.length; i++)
    {
        u32 carry2;

        carry2 = r.limb[i];
        r.limb[i] = (r.limb[i] << bits) & MAX_U32;
        r.limb[i] = (r.limb[i] | carry) & MAX_U32;
        carry = carry2 >> (word_size - bits);
    }

    if(carry != 0)
    {
        r.limb[r.length + limbs] = carry;
    }
}

BIG_INTEGER_API void big_integer_shift_right(big_integer *value1, u32 times, big_integer *result)
{
    int word_size;
    int limbs;
    int bits;
    int i;
    big_integer r;
    u32 carry;
}

BIG_INTEGER_API void big_integer_multiply(big_integer *multiplier, big_integer *multiplicand, big_integer *product)
{
    u32 i;
    u32 max_length;
    big_integer p;
    u32 limb;

    if(memory_is_little_endian())
    {
        max_length = multiplier->length + multiplicand->length;

        for(i = 0; i < max_length; i += 2)
        {
            u32 m1;
            u32 m2;
            u64_emulated prod;

            m1 = multiplier->limb[i];
            m2 = multiplicand->limb[i];
            
            u32_multiply(m1, m2, &prod);

            p.limb[i] = prod.low;
            p.limb[i + 1] = prod.high;
        }

        big_integer_copy(&p, product);
    }
    else
    {
        return;
    }
}

BIG_INTEGER_API void big_integer_divide(big_integer *value1, big_integer *value2, big_integer *result)
{
}

BIG_INTEGER_API void big_integer_from_ieee754(int exponent, int bias, int k, u64_emulated *fraction, big_integer *numerator, big_integer *denominator)
{
    /*
        Normal: (-1)^s * 1.F * 2^(E - bias)
        (-1)^s * 2^(E - bias) * 1.F
        (-1)^s * 2^(E - bias) * (2^52 + (F/2^52))
        n = (2^52 + F)
        d = 2^52
        (-1)^s * 2^(E - bias) * (n/d)
        n/d * 2^(E - bias)
        n/2^52 * 2^(E - bias)
        k = (52, E - bias)
        ((2^52 + F) * 2^(E - bias - k)) / 2^(52-k)
        (-1)^s * ((2^52 + F) * 2^(E - bias - k)) / 2^(52-k)


        Subnormal: (-1)^s * F * 2^(1 - bias)
        n = F
        d = 2^52
        k = -1022 == is_subnormal
        n * 2^(1 - bias - k)
        d = 2^(52 - k)
        (-1)^s * 2^(1 - bias - k) * n/d
        (-1)^s * 2^(1 - 1023 - -1022) * n/d 
        (-1)^s * n/d 
        (-1)^s * F/2^(1074)
    */

    bool is_subnormal;

    is_subnormal = k == -1022;

    if(is_subnormal)
    {
        big_integer_from_u64_emulated(fraction, numerator);
        big_integer_from_u32(1, denominator);
        big_integer_shift_left(denominator, 1074, denominator);
    }
    else
    {
        big_integer temporary;
        int e;

        e = exponent - bias - k;

        ASSERT(e >= 0);

       big_integer_from_u64_emulated(fraction, numerator);
       big_integer_from_u32(1, &temporary);
       big_integer_shift_left(&temporary, 52, &temporary);
       big_integer_add(numerator, &temporary, numerator);

       big_integer_from_u32(1, &temporary);
       big_integer_shift_left(&temporary, e, &temporary);
       big_integer_multiply(numerator, &temporary, numerator);

        e = 52 - k;

        ASSERT(e >= 0);

        big_integer_from_u32(1, denominator);
        big_integer_shift_left(denominator, e, denominator);
    }
}