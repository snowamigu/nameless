#include "library/platform.h"
#include "library/definitions.h"
#include "library/console.h"
#include "library/string.h"

INTERNAL void memory_copy(void *source, void *destination, u32 size)
{
    u8 *s = source;
    u8 *d = destination;
    u32 i;

    for(i = 0; i < size; i++)
    {
        *d++ = *s++;
    }
}

INTERNAL f64 double_negative_infinity()
{
    f64 value;
    u32 bytes[2];

    bytes[0] = 0;
    bytes[1] = 0xFFF00000;
    
    memory_copy(bytes, &value, sizeof(value));

    return value;
}

INTERNAL f64 double_infinity()
{
    f64 value;
    u32 bytes[2];

    bytes[0] = 0;
    bytes[1] = 0x7FF00000;
    
    memory_copy(bytes, &value, sizeof(value));

    return value;
}

INTERNAL f64 double_quiet_not_a_number()
{
    f64 value;
    u32 bytes[2];

    bytes[0] = 1;
    bytes[1] = 0x7FF80000;
    
    memory_copy(bytes, &value, sizeof(value));

    return value;
}

INTERNAL f64 double_signaling_not_a_number()
{
    f64 value;
    u32 bytes[2];

    bytes[0] = 1;
    bytes[1] = 0x7FF00000;
    
    memory_copy(bytes, &value, sizeof(value));

    return value;
}

INTERNAL f64 double_biggest_subnormal()
{
    f64 value;
    u32 bytes[2];

    bytes[0] = 0xFFFFFFFF;
    bytes[1] = 0x000FFFFF;
    
    memory_copy(bytes, &value, sizeof(value));

    return value;
}

INTERNAL void print(char *format, ...)
{
    va_list argument_list;
    char buffer[1024];
    u32 buffer_length;

    va_start(argument_list, format);
    string_format(&buffer_length, buffer, sizeof(buffer), format, argument_list);
    console_write(CONSOLE_HANDLE_OUTPUT, buffer, buffer_length);
    va_end(argument_list);
}

int main(int argument_count, char **argument_list)
{
    char buffer[512];
    u32 length;
    
    print("%f %f %f\n", 0.0, double_biggest_subnormal(), double_biggest_subnormal());

    return 0;

    print("%% Hello, world! %%\n");
    print("%% Hello, world! %%\n");
    print("%% Hello, world! %%\n");

    print("%2c %5c %10c\n", 'a', 'b', 'c');
    print("%-1c %-5c %-10c\n", 'a', 'b', 'c');
    print("%c %c %c\n", 'a', 'b', 'c');
    
    print("%d %i %d\n", 1234, 0, 1234);
    print("%d %i %d\n", 0, -1, 1234);
    print("%d %i %d\n", 0, -1, 1234);

    print("%f %f %f\n", 0.0, -0.0, 0.0);
    print("%f %f %f\n", double_infinity(), double_negative_infinity(), double_infinity());
    print("%f %f %f %f\n", -double_quiet_not_a_number(), double_quiet_not_a_number(), -double_signaling_not_a_number(), double_signaling_not_a_number());

    print("%o %o %o\n", 0, 8, 16);
    print("%o %o %o\n", 0, 8, 16);
    print("%o %o %o\n", 0, 8, 16);

    print("%p %p %p\n", 0, main, argument_list);
    print("%p %p %p\n", 0, main, argument_list);
    print("%p %p %p\n", 0, main, argument_list);

    print("%s %s %s\n", "abc", "def", "ghi");
    print("%s %s %s\n", "abc", "def", "ghi");
    print("%s %s %s\n", "abc", "def", "ghi");

    print("%u %u %u\n", 0, -1, 1234);
    print("%u %u %u\n", 0, -1, 1234);
    print("%u %u %u\n", 0, -1, 1234);

    print("%x %X %x\n", 0, -1, 255);
    print("%x %X %x\n", 0, -1, 255);
    print("%x %X %x\n", 0, -1, 255);

    return 0;
}