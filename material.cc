// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "shared.h"
#include "material.h"

#include "pbr.h"
#include "mat4.h"
#include "sphere.h"

//------------------------------------------------------------------------------
/**
*/
void
BSDF(const Material& material, Ray& ray, const vec3& point, const vec3& normal)
{
	float cosTheta = -dot(normalize(ray.m), normalize(normal));

	if (material.type != MaterialType::Dielectric)
	{
		float F0 = 0.04f;
		if (material.type == MaterialType::Conductor)
		{
			F0 = 0.95f;
		}

		// probability that a ray will reflect on a microfacet
		float F = FresnelSchlick(cosTheta, F0, material.roughness);

		float r = rng.fnext();

		if (r < F)
		{
			mat4 basis = TBN(normal);
			// importance sample with brdf specular lobe
			vec3 H = ImportanceSampleGGX_VNDF(rng.fnext(), rng.fnext(), material.roughness, ray.m, basis);
			vec3 reflected = reflect(ray.m, H);
			ray.b = point;
			ray.m = normalize(reflected);
		}
		else
		{
			float x = rng.fnext(-1, 1);
			float y = rng.fnext(-1, 1);
			float z = rng.fnext(-1, 1);
			vec3 v( x, y, z );
			ray.b = point;
			ray.m = normalize(normalize(normal) + normalize(v));
		}
	}
	else
	{
		vec3 outwardNormal;
		float niOverNt;
		vec3 refracted;
		float reflect_prob;
		float cosine;
		vec3 rayDir = ray.m;

		if (cosTheta <= 0)
		{
			outwardNormal = -normal;
			niOverNt = material.refractionIndex;
			cosine = cosTheta * niOverNt / len(rayDir);
		}
		else
		{
			outwardNormal = normal;
			niOverNt = 1.0 / material.refractionIndex;
			cosine = cosTheta / len(rayDir);
		}

		if (Refract(normalize(rayDir), outwardNormal, niOverNt, refracted))
		{
			// fresnel reflectance at 0 deg incidence angle
			float F0 = powf(material.refractionIndex - 1, 2) / powf(material.refractionIndex + 1, 2);
			reflect_prob = FresnelSchlick(cosine, F0, material.roughness);
		}
		else
		{
			reflect_prob = 1.0;
		}
		if (rng.fnext() < reflect_prob)
		{
			vec3 reflected = reflect(rayDir, normal);
			ray.b = point;
			ray.m = reflected;
		}
		else
		{
			ray.b = point;
			ray.m = refracted;
		}
	}
}
