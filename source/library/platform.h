#ifndef PLATFORM_H
#define PLATFORM_H

/* Detects the platform and architecture. Declares some definitions. */
#ifdef _WIN64
    #define WINDOWS
    #define ARCHITECTURE64
    #define EXPORT __declspec(dllexport)
    #define IMPORT __declspec(dllimport)
#else
    #ifdef _WIN32
        #define WINDOWS
        #define ARCHITECTURE64
        #define EXPORT __declspec(dllexport)
        #define IMPORT __declspec(dllimport)
    #else
        #error
    #endif
#endif

/* Requiments of the platform. */
typedef char check_char_size[(sizeof(char) < 1) ? (0) : (1)];
typedef char check_short_size[(sizeof(short) < 2) ? (0) : (1)];
typedef char check_long_size[(sizeof(long) < 4) ? (0) : (1)];
typedef char check_float_size[(sizeof(float) < 4) ? (0) : (1)];
typedef char check_double_size[(sizeof(double) < 8) ? (0) : (1)];

/* Some types. */
typedef char s8;
typedef unsigned char u8;
typedef int s16;
typedef unsigned int u16;
typedef long s32;
typedef unsigned long u32;
typedef u32 bool;
//typedef u32 uhalf; /* at least 32 bits */
typedef float f32;
typedef double f64;
typedef struct { u32 low; u32 high; } s64_emulated;
typedef struct { u32 low; u32 high; } u64_emulated;
typedef struct { u64_emulated low; u64_emulated high; } s128_emulated;
typedef struct { u64_emulated low; u64_emulated high; } u128_emulated;

#endif