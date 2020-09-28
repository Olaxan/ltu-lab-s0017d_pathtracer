#pragma once
#include "vec3.h"
#include <float.h>

enum class Shapes
{
	None,
	Sphere
};

struct HitResult
{
	// hit point
	vec3 p;
	// normal
	vec3 normal;
	// type of hit object
	Shapes shape = Shapes::None;
	// index of hit object
	size_t index;
	// intersection distance
	float dist = FLT_MAX;
};

