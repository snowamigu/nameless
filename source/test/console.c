#include "library/platform.h"
#include "library/definitions.h"
#include "library/console.h"

int main(int argument_count, char **argument_list)
{
    u32 console_error;
    char buffer[512];

    ASSERT((console_error = console_write(CONSOLE_HANDLE_OUTPUT, "Hello\n", 6)) == CONSOLE_ERROR_SUCCESS);
    ASSERT((console_error = console_error_get_message(console_error, buffer, sizeof(buffer))) == CONSOLE_ERROR_SUCCESS);
    ASSERT((console_error = console_error_get_message(console_error, buffer, 0)) == CONSOLE_ERROR_BUFFER_INSUFFICIENT);
    ASSERT((console_error = console_error_get_message(console_error, buffer, 1)) == CONSOLE_ERROR_BUFFER_INSUFFICIENT);
    
    return 0;
}