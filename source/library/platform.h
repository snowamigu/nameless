#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN64
    #define WINDOWS
    #define ARCHITECTURE64
    #define EXPORT __declspec(dllexport)
    #define IMPORT __declspec(dllimport)
#else
    #ifdef _WIN32
        #define WINDOWS
        #define ARCHITECTURE32
        #define EXPORT __declspec(dllexport)
        #define IMPORT __declspec(dllimport)
    #else
        #error
    #endif
#endif

#define INTERNAL static
#define GLOBAL static
#define LOCAL static

typedef char check_char_size[(sizeof(char) < 1) ? (-1) : (1)];
typedef char check_int_size[(sizeof(int) < 2) ? (-1) : (1)];
typedef char check_long_size[(sizeof(long) < 4) ? (-1) : (1)];
typedef char check_float_size[(sizeof(float) < 4) ? (-1) : (1)];
typedef char check_double_size[(sizeof(double) < 8) ? (-1) : (1)];

typedef char s8;
typedef unsigned char u8;
typedef int s16;
typedef unsigned int u16;
typedef long s32;
typedef unsigned int u32;
typedef u32 bool;
typedef s32 shalf;
typedef u32 uhalf;
typedef float f32;
typedef double f64;

#endif