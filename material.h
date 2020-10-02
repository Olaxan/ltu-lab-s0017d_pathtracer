#pragma once

#include "vec3.h"
#include "xrng.h"

enum class MaterialType
{
	Lambertian,
	Dielectric,
	Conductor
};

//------------------------------------------------------------------------------
/**
*/
struct Material
{
	
	/*
		type can be "Lambertian", "Dielectric" or "Conductor".
		Obviously, "lambertian" materials are dielectric, but we separate them here
		just because figuring out a good IOR for ex. plastics is too much work
	*/
	MaterialType type = MaterialType::Lambertian;
	vec3 color = {0.5f,0.5f,0.5f};
	float roughness = 0.75;

	// this is only needed for dielectric materials.
	float refractionIndex = 1.44;
};

//------------------------------------------------------------------------------
/**
	Scatter ray against material
*/
void BSDF(const Material& material, vec3& origin, vec3& dir, const vec3& point, const vec3& normal, xrng::xoshiro128_plus& rng);
