#define BIG_INTEGER_EXPORT
#include "big_integer.h"

BIG_INTEGER_API bool big_integer_error_is_set(uhalf *error)
{
    return (!error) ? (0) : (*error != BIG_INTEGER_ERROR_SUCCESS);
}

BIG_INTEGER_API void big_integer_error_set(uhalf *error, uhalf error_code)
{
    if(error)
    {
        *error = error_code;
    }
}

BIG_INTEGER_API bool big_integer_is_zero(uhalf *error, big_integer *integer_big)
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

BIG_INTEGER_API void big_integer_normalized(uhalf *error, big_integer *integer_big)
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

BIG_INTEGER_API int big_integer_compare(uhalf *error, big_integer *lhs, big_integer *rhs)
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

BIG_INTEGER_API void big_integer_zeroed(big_integer *integer_big)
{
    int i = 0;

    for(i = 0; i < BIG_INTEGER_MAX_LIMB_COUNT; i++)
    {
        integer_big->limb[i] = 0;
    }

    integer_big->length = 0;
}

BIG_INTEGER_API void big_integer_from_u32(uhalf *error, big_integer *integer_big, u32 value)
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

BIG_INTEGER_API void big_integer_from_u64_emulated(uhalf *error, big_integer *integer_big, u64_emulated value)
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

BIG_INTEGER_API void big_integer_copy(uhalf *error, big_integer *source, big_integer *destination)
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

BIG_INTEGER_API void big_integer_shift_left(uhalf *error, big_integer *integer_big, u32 times)
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

BIG_INTEGER_API void big_integer_shift_right(uhalf *error, big_integer *integer_big, int times)
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

BIG_INTEGER_API void big_integer_add(uhalf *error, big_integer *lhs, big_integer *rhs, big_integer *result)
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

BIG_INTEGER_API void big_integer_subtract(uhalf *error, big_integer *lhs, big_integer *rhs, big_integer *result)
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

BIG_INTEGER_API void big_integer_multiply_u32(uhalf *error, big_integer *lhs, u32 rhs, big_integer *result)
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

BIG_INTEGER_API void big_integer_division_remainder(uhalf *error, big_integer *numerator, big_integer *denominator, big_integer *quotient, big_integer *remainder)
{
    return;
}
