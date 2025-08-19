#define IEEE754_EXPORT
#include "ieee754.h"
#include "memory.h"

IEEE754_API void ieee754_from_double(f64 value, ieee754_double *ieee754)
{
    ieee754_double_union u;

    if(memory_is_little_endian())
    {
        u.d = value;
        ieee754->sign = (u.i[1] >> 31) & 1;
        ieee754->exponent = (u.i[1] >> 20) & 0x7FF;
        ieee754->fraction.low = u.i[0];
        ieee754->fraction.high = u.i[1] & 0xFFFFF;
        ieee754->is_zero = !ieee754->exponent && !ieee754->fraction.low && !ieee754->fraction.high;
        ieee754->is_infinity = (ieee754->exponent == 0x7FF) && !ieee754->fraction.low && !ieee754->fraction.high;
        ieee754->is_quiet_not_a_number = (ieee754->exponent == 0x7FF) && ((ieee754->fraction.high >> 19) & 1) && ((ieee754->fraction.low > 0) || ((ieee754->fraction.high << 13) > 0));
        ieee754->is_signaling_not_a_number = (ieee754->exponent == 0x7FF) && !ieee754->fraction.high && (ieee754->fraction.low == 1);
        ieee754->is_subnormal = !ieee754->exponent && !(!ieee754->fraction.low && !ieee754->fraction.high);

        if(!ieee754->is_subnormal)
        {
            ieee754->fraction.high |= 1 << 20;
        }
    }
    else
    {
        return;
    }

    return;
}