#ifndef PHYSICS_H
#define PHYSICS_H

#include "library/debug.h"
#include "library/release.h"
#include "library/platform.h"

#ifdef PHYSICS_EXPORT
    #define PHYSICS_API EXPORT
#else
    #ifdef PHYSICS_IMPORT
        #define PHYSICS_API IMPORT
    #else
        #define PHYSICS_API
    #endif
#endif

typedef double real;

typedef struct
{
    real x;
    real y;
} physics_vector2;

typedef struct
{
    physics_vector2 position;
    physics_vector2 velocity;
    physics_vector2 acceleration;
    real mass;
} physics_body_2d;

PHYSICS_API real physics_square_root(real value);
PHYSICS_API void physics_vector2_add(physics_vector2 *vector1, physics_vector2 *vector2, physics_vector2 *result);
PHYSICS_API void physics_vector2_subtract(physics_vector2 *vector1, physics_vector2 *vector2, physics_vector2 *result);
PHYSICS_API void physics_vector2_multiply_by_scalar(physics_vector2 *vector, real scalar, physics_vector2 *result);
PHYSICS_API void physics_vector2_divide_by_scalar(physics_vector2 *vector, real scalar, physics_vector2 *result);
PHYSICS_API void physics_vector2_inverse(physics_vector2 *vector, physics_vector2 *result);
PHYSICS_API real physics_vector2_length(physics_vector2 *vector);
PHYSICS_API void physics_vector2_normalize(physics_vector2 *vector, physics_vector2 *result);
PHYSICS_API real physics_vector2_scalar_product(physics_vector2 *vector1, physics_vector2 *vector2);
PHYSICS_API void physics_vector2_perpendicular(physics_vector2 *vector, physics_vector2 *result);
PHYSICS_API void physics_body_2d_apply_force(physics_body_2d *body, physics_vector2 *force, physics_body_2d *result);
PHYSICS_API void physics_body_2d_update(physics_body_2d *body, real delta_time, physics_body_2d *result);

#endif