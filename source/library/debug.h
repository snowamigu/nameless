#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
    #define DEBUG_ASSERT(expression) do { if(!(expression)) { *(int *)0 = 0; } } while(0)
#else
    #define DEBUG_ASSERT(expression)
#endif

#endif