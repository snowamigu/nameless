#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define INTERNAL static
#define GLOBAL static
#define LOCAL static
#define MAX_U32 0xFFFFFFFF
#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

#ifndef DEBUG
    #define ASSERT(expression)
#else
    #define ASSERT(expression) do { if(!(expression)) { *(int *)0 = 0; } } while(0)
#endif

#endif