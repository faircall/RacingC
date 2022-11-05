#include "r_math.h"

float deg_to_rad(float deg)
{
    float result = MPI*deg/180.0f;
    return result;
}

float rad_to_deg(float rad)
{
    float result = 180.0f*rad/MPI;
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

float vec_mag(Vec2 a)
{
    return sqrt(a.x*a.x + a.y*a.y);
}

float vec_dot(Vec2 a, Vec2 b)
{
    return a.x*b.x + a.y*b.y;
}

float angle_between(Vec2 a, Vec2 b)
{
    float dot = vec_dot(a, b);
    float cos_theta = dot / (vec_mag(a) * vec_mag(b));
    float angle = rad_to_deg(acos(cos_theta));
    return angle;
    
}
