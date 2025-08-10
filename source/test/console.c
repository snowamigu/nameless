#include "library/platform.h"
#include "library/definitions.h"
#include "library/console.h"

int main(int argument_count, char **argument_list)
{
    u32 console_error = CONSOLE_ERROR;
    char buffer[512];

    console_error = console_write(CONSOLE_HANDLE_OUTPUT, "Hello\n", 6);

    if(console_error == CONSOLE_ERROR_INVALID_PARAMETER)
    {
        u32 console_error2 = console_error_get_message(console_error, buffer, sizeof(buffer));
        return 1;
    }
    else if(console_error != CONSOLE_ERROR_SUCCESS)
    {
        u32 console_error2 = console_error_get_message(console_error, buffer, sizeof(buffer));
        return 1;
    }

    return 0;
}