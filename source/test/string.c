#include "library/platform.h"
#include "library/definitions.h"
#include "library/hardware.h"
#include "library/software.h"

typedef union
{
    f64 d;
    u32 i[2];
} ieee754_double_union;

INTERNAL f64 build_double(u32 low, u32 high)
{
    ieee754_double_union u;

    u.i[0] = low;
    u.i[1] = high;

    return u.d;
}

INTERNAL u32 print(char *format, ...)
{
    va_list argument_list;
    char buffer[512];
    u32 buffer_length;

    va_start(argument_list, format);
    buffer_length = string_format(buffer, format, argument_list);
    console_write(CONSOLE_HANDLE_OUTPUT, buffer, buffer_length);
    va_end(argument_list);

    return buffer_length;
}

int main(int argument_count, char **argument_list)
{
    print("%% Hello, world! %%\n");
    print("%% Hello, world! %%\n");
    print("%% Hello, world! %%\n");

    print("%c %c %c\n", 'a', 'b', 'c');
    print("%c %c %c\n", 'a', 'b', 'c');
    print("%c %c %c\n", 'a', 'b', 'c');

    print("%d %i %d\n", 0, -1, 1234);
    print("%d %i %d\n", 0, -1, 1234);
    print("%d %i %d\n", 0, -1, 1234);

    print("%f %f %f\n", 0.0, 0.0, 0.0);
    print("%f %f %f\n", build_double(0, 0x7FF00000), build_double(0, 0xFFF00000), build_double(0, 0x7FF00000));
    print("%f %f %f\n", build_double(1, 0x7FF00000), build_double(1, 0x7FF80000), build_double(1, 0x7FF00000));
    print("%f %f %f\n", build_double(0, 0x40370000), build_double(0, 0x40370000), build_double(0, 0x40370000));
    
#if 0
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
#endif

    return 0;
}