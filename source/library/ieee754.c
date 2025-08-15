#define IEEE754_EXPORT
#include "ieee754.h"

IEEE754_API bool ieee754_error_is_set(uhalf *error)
{
    return (!error) ? (0) : (*error != IEEE754_ERROR_SUCCESS);
}

IEEE754_API void ieee754_error_set(uhalf *error, uhalf error_code)
{
    if(error)
    {
        *error = error_code;
    }
}

IEEE754_API void ieee754_error_get_message(uhalf *error, u32 error_code, char *buffer, u32 buffer_size)
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

IEEE754_API bool ieee754_double_is_infinity(uhalf *error, bool *sign, f64 value)
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

IEEE754_API bool ieee754_double_is_not_a_number(uhalf *error, f64 value)
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

IEEE754_API void ieee754_decode_double(uhalf *error, f64 value, bool *sign, int *exponent2, u64_emulated *mantissa)
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
