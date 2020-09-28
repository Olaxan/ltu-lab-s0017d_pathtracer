// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "shared.h"
#include "material.h"

#include "pbr.h"
#include "mat4.h"

//------------------------------------------------------------------------------
/**
*/
void
BSDF(Ray& ray, const Material& material, const vec3& point, const vec3& normal)
{
	const float cosTheta = -dot(normalize(ray.m), normalize(normal));

	if (material.type != MaterialType::Dielectric)
	{
		const float F0 = material.type == MaterialType::Conductor ? 0.95f : 0.04f;

		// probability that a ray will reflect on a microfacet
		const float F = FresnelSchlick(cosTheta, F0, material.roughness);

		const float r = rng.fnext();

		if (r < F)
		{
			const mat4 basis = TBN(normal);
			// importance sample with brdf specular lobe
			const vec3 H = ImportanceSampleGGX_VNDF(rng.fnext(), rng.fnext(), material.roughness, ray.m, basis);
			const vec3 reflected = reflect(ray.m, H);
			ray.b = point;
			ray.m = normalize(reflected);
		}
		else
		{
			const float x = rng.fnext(-1, 1);
			const float y = rng.fnext(-1, 1);
			const float z = rng.fnext(-1, 1);
			const vec3 v( x, y, z );
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
