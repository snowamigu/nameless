#include "library/debug.h"
#include "library/release.h"
#include "library/platform.h"
#include "library/hardware.h"
#include "library/string.h"

INTERNAL u32 print(char *format, ...)
{
    va_list argument_list;
    char buffer[512];
    u32 buffer_length;

    va_start(argument_list, format);
    buffer_length = string_format(format, buffer, argument_list);
    hardware_write(HARDWARE_HANDLE_CONSOLE_OUTPUT, buffer, buffer_length);
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

    print("%f %f %f\n", 0, -1.0, 1.234);
    print("%f %f %f\n", 0, -1.0, 1.234);
    print("%f %f %f\n", 0, -1.0, 1.234);

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