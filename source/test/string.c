#include "library/platform.h"
#include "library/definitions.h"
#include "library/console.h"
#include "library/string.h"

INTERNAL void print(char *format, ...)
{
    va_list argument_list;
    char buffer[1024];
    u32 count = 0;

    va_start(argument_list, format);
    string_format(&count, buffer, sizeof(buffer), format, argument_list);
    console_write(CONSOLE_HANDLE_OUTPUT, buffer, count);
    va_end(argument_list);

    return;
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

    print("%f %f %f\n", 1.0, -1.0, 2.0);

    print("%f %f %f\n", 0.0, 1.0, 1.234);
    print("%f %f %f\n", 0.0, 1.0, 1.234);

#if 0

    print("%o %o %o\n", 0, 8, 16);
    print("%o %o %o\n", 0, 8, 16);
    print("%o %o %o\n", 0, 8, 16);

    print("%p %p %p\n", 0, main, argument_list);
    print("%p %p %p\n", 0, main, argument_list);
    print("%p %p %p\n", 0, main, argument_list);

    print("%s %s %s\n", "ABC", "DEF", "GHI");
    print("%s %s %s\n", "ABC", "DEF", "GHI");
    print("%s %s %s\n", "ABC", "DEF", "GHI");

    print("%u %u %u\n", 0, -1, 1234);
    print("%u %u %u\n", 0, -1, 1234);
    print("%u %u %u\n", 0, -1, 1234);

    print("%x %X %x\n", 0, -1, 255);
    print("%x %X %x\n", 0, -1, 255);
    print("%x %X %x\n", 0, -1, 255);
#endif
   
    return 0;
}