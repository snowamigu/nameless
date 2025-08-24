#ifndef MATH_H
#define MATH_H

#include "library/debug.h"
#include "library/release.h"
#include "library/platform.h"

#ifdef MATH_EXPORT
    #define MATH_API EXPORT
#else
    #ifdef MATH_IMPORT
        #define MATH_API IMPORT
    #else
        #define MATH_API
    #endif
#endif

#endif