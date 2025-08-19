#define MEMORY_EXPORT
#include "memory.h"

MEMORY_API bool memory_is_little_endian()
{
    unsigned int i = 1;
    unsigned char *p = (unsigned char *)&i;

    return *p == 1;
}

MEMORY_API void memory_zeroed(void *value, u32 size)
{
    u8 *v = value;
    u32 i;

    if(!size)
    {
        return;
    }

    for(i = 0; i < size; i++)
    {
        *v++ = 0;
    }

    return;
}

MEMORY_API int memory_count_trailing_zeros_u32(u32 value)
{
    int result = 0;
    int i;

    for(i = 0; (i < 32) && !(value & (1 << i)); i++)
    {
        result++;
    }

    return result;
}

MEMORY_API int memory_count_trailing_zeros_u64_emulated(u64_emulated value)
{
    int result = 0;
    int i;

    for(i = 0; (i < 32) && !(value.low & (1 << i)); i++)
    {
        result++;
    }

    for(i = 0; (i < 64) && (i >= 32) && !(value.high & (1 << i)); i++)
    {
        result++;
    }

    return result;
}

MEMORY_API int memory_count_leading_zeros_u32(u32 value)
{
    int result = 0;
    int i;

    for(i = 31; (i >= 0) && !(value & (1 << i)); i--)
    {
        result++;
    }

    return result;
}