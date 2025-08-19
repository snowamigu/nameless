#include "library/platform.h"
#include "library/definitions.h"
#include "library/console.h"

int main(int argument_count, char **argument_list)
{
    char buffer[512];

    console_write(CONSOLE_HANDLE_OUTPUT, "Hello\n", 6);
    
    return 0;
}