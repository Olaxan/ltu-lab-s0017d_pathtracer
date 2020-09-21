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
    {
        // Reserve characters for naming this object something!
        name = new char[256];
        name[0] = 'U';
        name[1] = 'n';
        name[2] = 'n';
        name[3] = 'a';
        name[4] = 'm';
        name[5] = 'e';
        name[6] = 'd';
        name[7] = '\0';

        purpose = std::string("I don't have a purpose at the moment, but hopefully the programmer that overrides me will give me one. :)");
    }

    virtual ~Object()
    {
        // clean up name!
        delete[] name;
    }

    virtual bool Intersect(HitResult& hit, Ray ray, float maxDist) = 0;
    virtual Color GetColor() = 0;
    virtual Ray ScatterRay(Ray ray, vec3 point, vec3 normal) = 0;

private:
    volatile bool isBigObject = false;
    volatile char* name;
    std::string purpose;
};
