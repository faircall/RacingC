#include "r_math.h"

float deg_to_rad(float deg)
{
    float result = MPI*deg/180.0f;
    return result;
}

Vec2 init_vec2(float x, float y)
{
    Vec2 result;
    result.x = x;
    result.y = y;
    return result;
}

float float_min(float x, float y)
{
    if (x <= y) {
	return x;
    }
    return y;
}

float float_max(float x, float y)
{
    if (x >= y) {
	return x;
    }
    return y;
}

float distance(float x, float y)
{
    float result;
    result = sqrt((x-y)*(x-y));
    return result;
}

Vec2 vec_normalize(Vec2 in)
{
    Vec2 result = init_vec2(0.0f, 0.0f);
    float mag = sqrt(in.x*in.x + in.y*in.y);
    if (mag != 0.0f) {
	result.x = in.x/mag;
	result.y = in.y/mag;
    } else {
	result.x = 0.0f;
	result.y = 0.0f;
    }

    return result;
}
