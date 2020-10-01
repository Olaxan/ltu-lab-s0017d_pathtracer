#pragma once

#include <stdlib.h>
#include <time.h>

#include "object.h"
#include "mat4.h"
#include "material.h"

// a spherical object
class Sphere
{
public:
	const float radius;
	const vec3 center;
	const size_t material_index;

	Sphere(float radius, const vec3& center, size_t material_index) : 
		radius(radius),
		center(center),
		material_index(material_index)
	{ }

	~Sphere() = default;

	bool intersect(const vec3& point, const vec3& dir, float maxDist, HitResult& hit)
	{
		const vec3 oc = point - this->center;
		const float b = dot(oc, dir);
	
		// early out if this->is "behind" ray
		if (b > 0)
			return false;

		const float a = dot(dir, dir);
		const float c = dot(oc, oc) - this->radius * this->radius;
		const float discriminant = b * b - a * c;

		if (discriminant > 0)
		{
			const float minDist = 0.001f;
			const float div = 1.0f / a;
			const float sqrtDisc = sqrt(discriminant);
			const float temp = (-b - sqrtDisc) * div;
			const float temp2 = (-b + sqrtDisc) * div;

			if (temp < maxDist && temp > minDist)
			{
				const vec3 p = { point + dir * temp };
				hit.p = p;
				hit.normal = (p - this->center) * (1.0f / this->radius);
				hit.shape = Shapes::Sphere;
				hit.dist = temp;
				return true;
			}
			if (temp2 < maxDist && temp2 > minDist)
			{
				const vec3 p = { point + dir * temp2 };
				hit.p = p;
				hit.normal = (p - this->center) * (1.0f / this->radius);
				hit.shape = Shapes::Sphere;
				hit.dist = temp2;
				return true;
			}
		}

		return false;
	}
};
