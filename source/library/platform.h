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
        #error "Platform not supported."
    #endif
#endif

typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef long s32;
typedef unsigned long u32;
typedef u32 bool;
typedef s32 shalf;
typedef u32 uhalf;
typedef float f32;
typedef double f64;
typedef struct { u32 low; u32 high; } s64_emulated;
typedef struct { u32 low; u32 high; } u64_emulated;

#endif