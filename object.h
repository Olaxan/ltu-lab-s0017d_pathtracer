#pragma once
#include "ray.h"
#include "color.h"
#include <float.h>
#include <string>

class Object;

//------------------------------------------------------------------------------
/**
*/
struct HitResult
{
    // hit point
    vec3 p;
    // normal
    vec3 normal;
    // hit object, or nullptr
    Object* object = nullptr;
    // intersection distance
    float t = FLT_MAX;
};

//------------------------------------------------------------------------------
/**
*/
class Object
{
public:
    Object()
    { }

    virtual ~Object()
    { }

    virtual bool Intersect(HitResult& hit, Ray ray, float maxDist) = 0;
    virtual Color GetColor() = 0;
    virtual Ray ScatterRay(Ray ray, vec3 point, vec3 normal) = 0;
};
