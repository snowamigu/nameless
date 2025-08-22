#include "library/platform.h"
#include "library/definitions.h"
#include "library/console.h"

int main(int argument_count, char **argument_list)
{
    console_write(CONSOLE_HANDLE_OUTPUT, "Hello\n", 6);
    
    return 0;
}