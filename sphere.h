#pragma once

#include "object.h"
#include <stdlib.h>
#include <time.h>

#include "mat4.h"
#include "pbr.h"
#include "ray.h"
#include "material.h"

// a spherical object
class Sphere : public Object
{
public:
	const float radius;
	const vec3 center;
	const Material material;

	Sphere(float radius, vec3 center, Material material) : 
		radius(radius),
		center(center),
		material(material)
	{ }

	~Sphere()
	{ }

	Color GetColor() override
	{
		return material.color;
	}

	bool Intersect(HitResult& hit, const Ray& ray, float maxDist) override
	{
		vec3 oc = ray.b - this->center;
		vec3 dir = ray.m;
		float b = dot(oc, dir);
	
		// early out if sphere is "behind" ray
		if (b > 0)
			return false;

		float a = dot(dir, dir);
		float c = dot(oc, oc) - this->radius * this->radius;
		float discriminant = b * b - a * c;

		if (discriminant > 0)
		{
			constexpr float minDist = 0.001f;
			float div = 1.0f / a;
			float sqrtDisc = sqrt(discriminant);
			float temp = (-b - sqrtDisc) * div;
			float temp2 = (-b + sqrtDisc) * div;

			if (temp < maxDist && temp > minDist)
			{
				vec3 p = ray.PointAt(temp);
				hit.p = p;
				hit.normal = (p - this->center) * (1.0f / this->radius);
				hit.t = temp;
				return true;
			}
			if (temp2 < maxDist && temp2 > minDist)
			{
				vec3 p = ray.PointAt(temp2);
				hit.p = p;
				hit.normal = (p - this->center) * (1.0f / this->radius);
				hit.t = temp2;
				return true;
			}
		}

		return false;
	}

	void ScatterRay(Ray& ray, const vec3& point, const vec3& normal) override
	{
		BSDF(this->material, ray, point, normal);
	}

};
