#define PHYSICS_EXPORT
#include "physics.h"

#define PHYSICS_MAX_ITERATION_NEWTON_RAPHSON 5

INTERNAL void physics_memory_zeroed(void *destination, u32 size)
{
    u8 *d;
    u32 i;

    d = destination;

    for(i = 0; i < size; i++)
    {
        *d = 0;
    }
}

PHYSICS_API real physics_square_root(real value)
{
    real result;
    int i;

    result = value;

    for(i = 0; i < PHYSICS_MAX_ITERATION_NEWTON_RAPHSON; i++)
    {
        result = 0.5 * (result + (value / result));
    }

    return result;
}

PHYSICS_API void physics_vector2_add(physics_vector2 *vector1, physics_vector2 *vector2, physics_vector2 *result)
{
    result->x = vector1->x + vector2->x;
    result->y = vector1->y + vector2->y;
}

PHYSICS_API void physics_vector2_subtract(physics_vector2 *vector1, physics_vector2 *vector2, physics_vector2 *result)
{
    result->x = vector1->x - vector2->x;
    result->y = vector1->y - vector2->y;
}

PHYSICS_API void physics_vector2_multiply_by_scalar(physics_vector2 *vector, real scalar, physics_vector2 *result)
{
    result->x = vector->x * scalar;
    result->y = vector->y * scalar;
}

PHYSICS_API void physics_vector2_divide_by_scalar(physics_vector2 *vector, real scalar, physics_vector2 *result)
{
    result->x = vector->x / scalar;
    result->y = vector->y / scalar;
}

PHYSICS_API void physics_vector2_inverse(physics_vector2 *vector, physics_vector2 *result)
{
    result->x = -vector->x;
    result->y = -vector->y;
}

PHYSICS_API real physics_vector2_length(physics_vector2 *vector)
{
    return physics_square_root((vector->x * vector->x) + (vector->y * vector->y));
}

PHYSICS_API void physics_vector2_normalize(physics_vector2 *vector, physics_vector2 *result)
{
    real length;

    length = physics_vector2_length(vector);

    result->x = vector->x / length;
    result->y = vector->y / length;
}

PHYSICS_API real physics_vector2_scalar_product(physics_vector2 *vector1, physics_vector2 *vector2)
{
    return (vector1->x * vector2->x) + (vector1->y * vector2->y);
}

PHYSICS_API void physics_vector2_perpendicular(physics_vector2 *vector, physics_vector2 *result)
{
    result->x = -vector->y;
    result->y = vector->x;
}

PHYSICS_API void physics_body_2d_apply_force(physics_body_2d *body, physics_vector2 *force, physics_body_2d *result)
{
    /*
        F = m * a
        a = F / m
    */
    real body_inverse_mass;

    physics_memory_zeroed(result, sizeof(physics_body_2d));

    if(body->mass > 0)
    {
        body_inverse_mass = 1.0 / body->mass;

        result->acceleration.x += force->x * body_inverse_mass;
        result->acceleration.y += force->y * body_inverse_mass;
    }
}

PHYSICS_API void physics_body_2d_update(physics_body_2d *body, real delta_time, physics_body_2d *result)
{
    /*
        Euler integration
    */
   
    *result = *body;
    
    result->velocity.x += body->acceleration.x * delta_time;
    result->velocity.y += body->acceleration.y * delta_time;

    result->position.x += body->velocity.x * delta_time;
    result->position.y += body->velocity.y * delta_time;

    physics_memory_zeroed(&result->acceleration, sizeof(physics_vector2));
}