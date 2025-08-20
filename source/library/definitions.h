#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define INTERNAL static
#define GLOBAL static
#define LOCAL static
#define MAX_U8 0xFF
#define MAX_U16 0xFFFF
#define MAX_U32 0xFFFFFFFF
#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ABSOLUTE(x) (((x) < 0) ? (-(x)) : (x))

#ifndef DEBUG
    #define ASSERT(expression)
#else
    #define ASSERT(expression) do { if(!(expression)) { *(int *)0 = 0; } } while(0)
#endif

#endif