#pragma once
#include "vec3.h"
#include "color.h"

//------------------------------------------------------------------------------
/**
*/
class Ray
{
public:
	Ray() :
		b(vec3()),
		m(vec3()),
		c({1, 1, 1}),
		f(false)
	{ }

	Ray(vec3 startpoint, vec3 dir) :
		b(startpoint),
		m(dir),
		c({1, 1, 1}),
		f(false)
	{ }

	~Ray()
	{

	}

	vec3 PointAt(float t) const
	{
		return {b + m * t};
	}

	// beginning of ray
	vec3 b;
	// magnitude and direction of ray
	vec3 m;
	// color of ray
	Color c;
	// ray finished (out of bounds)
	bool f;
};
