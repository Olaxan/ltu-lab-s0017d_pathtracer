// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "shared.h"
#include "raytracer.h"

//------------------------------------------------------------------------------
/**
*/
Raytracer::Raytracer(unsigned w, unsigned h, std::vector<Color>& frameBuffer, unsigned rpp, unsigned bounces) :
	frameBuffer(frameBuffer),
	rpp(rpp),
	bounces(bounces),
	width(w),
	height(h),
	view(identity()),
	frustum(identity())
{
	// empty
}

Raytracer::~Raytracer()
{
	for (Object* p : objects)
		delete p;
}

//------------------------------------------------------------------------------
/**
*/
void
Raytracer::Raytrace() const
{
	vec3 cam_pos = get_position(this->view);

	for (int x = 0; x < this->width; ++x)
	{
		for (int y = 0; y < this->height; ++y)
		{
			Color color;
			for (int i = 0; i < this->rpp; ++i)
			{
				float u = ((float(x + rng.fnext()) * (1.0f / this->width)) * 2.0f) - 1.0f;
				float v = ((float(y + rng.fnext()) * (1.0f / this->height)) * 2.0f) - 1.0f;

				vec3 direction = vec3(u, v, -1.0f);
				direction = transform(direction, this->frustum);
				
				Ray ray(cam_pos, direction);
				color += this->TracePath(ray, 0);
			}

			// divide by number of samples per pixel, to get the average of the distribution
			color.r /= this->rpp;
			color.g /= this->rpp;
			color.b /= this->rpp;

			this->frameBuffer[y * this->width + x] += color;
		}
	}
}

//------------------------------------------------------------------------------
/**
 * @parameter n - the current bounce level
*/
Color
Raytracer::TracePath(const Ray& ray, unsigned n) const
{
	vec3 hitPoint;
	vec3 hitNormal;
	Object* hitObject = nullptr;
	float distance = FLT_MAX;

	if (Raycast(ray, hitPoint, hitNormal, hitObject, distance))
	{
		Ray scatteredRay(hitObject->ScatterRay(ray, hitPoint, hitNormal));
		if (n < this->bounces)
		{
			return hitObject->GetColor() * this->TracePath(scatteredRay, n + 1);
		}

		if (n == this->bounces)
		{
			return {0,0,0};
		}
	}

	return this->Skybox(ray.m);
}

//------------------------------------------------------------------------------
/**
*/
bool
Raytracer::Raycast(const Ray& ray, vec3& hitPoint, vec3& hitNormal, Object*& hitObject, float& distance) const
{
	bool isHit = false;
	HitResult closestHit;
	int numHits = 0;
	HitResult hit;

	const auto& world = this->objects;

	for (size_t i = 0; i < world.size(); ++i)
	{
		if (world[i]->Intersect(hit, ray, closestHit.t))
		{
			assert(hit.t < closestHit.t);
			closestHit = hit;
			closestHit.object = world[i];
			isHit = true;
			numHits++;
		}
	}

	hitPoint = closestHit.p;
	hitNormal = closestHit.normal;
	hitObject = closestHit.object;
	distance = closestHit.t;
	
	return isHit;
}

//------------------------------------------------------------------------------
/**
*/
Color
Raytracer::Skybox(vec3 direction) const
{
	float t = 0.5 * (direction.y + 1.0);
	vec3 vec = vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
    	return {(float)vec.x, (float)vec.y, (float)vec.z};	
}

//------------------------------------------------------------------------------
/**
*/
void
Raytracer::Clear()
{
	for (auto& color : this->frameBuffer)
	{
		color.r = 0.0f;
		color.g = 0.0f;
		color.b = 0.0f;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Raytracer::UpdateMatrices()
{
	mat4 inverseView = inverse(this->view); 
	mat4 basis = transpose(inverseView);
	this->frustum = basis;
}
