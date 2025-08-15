#define MEMORY_EXPORT
#include "memory.h"

MEMORY_API bool memory_error_is_set(uhalf *error)
{
    return (!error) ? (0) : (*error != MEMORY_ERROR_SUCCESS);
}

MEMORY_API void memory_error_set(uhalf *error, u32 error_code)
{
    if(error)
    {
        *error = error_code;
    }
}

MEMORY_API int memory_count_leading_zeros(uhalf *error, u32 value)
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

MEMORY_API bool memory_is_little_endian()
{
    unsigned int x = 1;
    unsigned char *p = (unsigned char *)&x;

    return *p == 1;
}