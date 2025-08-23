#include "library/debug.h"
#include "library/release.h"
#include "library/platform.h"
#include "library/hardware.h"

int main(int argument_count, char **argument_list)
{
    hardware_write(HARDWARE_HANDLE_CONSOLE_OUTPUT, "Hello\n", 6);
    
    return 0;
}