#pragma once
#include "vec3.h"
#include "mat4.h"
#include <math.h>

//------------------------------------------------------------------------------
/**
*/
inline float
FresnelSchlick(float cosTheta, float F0, float roughness)
{
	return F0 + (fmax(1.0f - roughness, F0) - F0) * pow(2, ((-5.55473f * cosTheta - 6.98316f) * cosTheta));
}

//------------------------------------------------------------------------------
/**
*/
inline vec3
ImportanceSampleGGX_VNDF(float u1, float u2, float roughness, const vec3& V, const mat4& basis)
{
    const float alpha = roughness * roughness;

    const vec3 Ve = -vec3(dot(V, get_row0(basis)), dot(V, get_row2(basis)), dot(V, get_row1(basis)));

    const vec3 Vh = normalize(vec3(alpha * Ve.x, alpha * Ve.y, Ve.z));

    const float lensq = Vh.x * Vh.x + Vh.y * Vh.y;

    const vec3 T1 = lensq > 0.0f ? vec3(-Vh.y, Vh.x, 0.0f) * (1 / sqrtf(lensq)) : vec3(1.0f, 0.0f, 0.0f);
    const vec3 T2 = cross(Vh, T1);

    const float r = sqrt(u1);
    const float phi = 2.0f * MPI * u2;
    const float t1 = r * cos(phi);
    const float s = 0.5 * (1.0 + Vh.z);
    const float t1sq = (t1 * t1);
    const float t2 = (1.0 - s) * sqrtf(1.0 - t1sq) + s * (r * sin(phi));

    const vec3 Nh = T1 * t1 + T2 * t2 + Vh * sqrtf(fmaxf(0.0f, 1.0f - t1sq - (t2 * t2)));

    // Tangent space H
    const vec3 Ne = vec3(alpha * Nh.x, fmaxf(0.0f, Nh.z), alpha * Nh.y);

    // World space H
    return normalize(transform(Ne, basis));
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Refract(const vec3& v, const vec3& n, float niOverNt, vec3& refracted)
{
    const vec3 uv = normalize(v);
    const float dt = dot(uv, n);
    const float discriminant = 1.0f - niOverNt * niOverNt * (1.0f - dt * dt);
    if (discriminant > 0)
    {
        refracted = ((uv - n * dt) * niOverNt) - (n * sqrt(discriminant));
        return true;
    }

    return false;
}
