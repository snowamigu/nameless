#include "library/platform.h"
#include "library/definitions.h"
#include "library/console.h"
#include "library/string.h"

INTERNAL void print(char *format, ...)
{
    va_list argument_list;
    char buffer[1024];
    u32 buffer_length;
    uhalf string_error;
    uhalf console_error;

    va_start(argument_list, format);
    ASSERT((string_error = string_format(&buffer_length, buffer, sizeof(buffer), format, argument_list)) == STRING_ERROR_SUCCESS);
    ASSERT((console_error = console_write(CONSOLE_HANDLE_OUTPUT, buffer, buffer_length)) == CONSOLE_ERROR_SUCCESS);
    va_end(argument_list);
}

int main(int argument_count, char **argument_list)
{
    uhalf console_error;
    uhalf string_error;
    char buffer[512];
    u32 length;

    ASSERT(((string_error = string_length(&length, "Hello\n")) == STRING_ERROR_SUCCESS) && (length == 6));

    print("%% Hello, world! %%\n");
    print("%% Hello, world! %%\n");
    print("%% Hello, world! %%\n");

    print("%2c %5c %10c\n", 'a', 'b', 'c');
    print("%-1c %-5c %-10c\n", 'a', 'b', 'c');
    print("%c %c %c\n", 'a', 'b', 'c');
    
    print("%d %i %d\n", 1234, 0, 1234);
    print("%d %i %d\n", 0, -1, 1234);
    print("%d %i %d\n", 0, -1, 1234);

    print("%f %f %f\n", 0.0, -1.0, 1.234);
    print("%f %f %f\n", 0.0, -1.0, 1.234);
    print("%f %f %f\n", 0.0, -1.0, 1.234);

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