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
	rays.reserve(w * h * rpp);
	results.resize(w * h * rpp);
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
Raytracer::trace()
{
	vec3 cam_pos = get_position(this->view);

	size_t ray_count = width * height * rpp;

	for (int i = 0; i < ray_count; i++)
	{
		int x = (i / rpp) % width;
		int y = (i / rpp) / width;

		float u = ((float(x + rng.fnext()) * (1.0f / this->width)) * 2.0f) - 1.0f;
		float v = ((float(y + rng.fnext()) * (1.0f / this->height)) * 2.0f) - 1.0f;

		vec3 direction = vec3(u, v, -1.0f);
		direction = transform(direction, this->frustum);
		
		rays[i] = Ray(cam_pos, direction);	
	}

	int shadow;
	for (int b = 0; b < bounces; b++)
	{
		shadow = (b < bounces - 1);

		for (int r = 0; r < ray_count; r++)
		{
			if (rays[r].f)
				continue;

			if (raycast(r))
			{
				auto& res = results[r];
				rays[r].c *= res.object->GetColor() * shadow;
				res.object->ScatterRay(rays[r], res.p, res.normal);
			}
			else
			{
				rays[r].f = true;
				rays[r].c *= skybox(rays[r].m);
			}
		}
	}

	for (int i = 0; i < frameBuffer.size(); i++)
	{
		for (int j = 0; j < rpp; j++)
		{
			frameBuffer[i] += rays[i * rpp + j].c;
		}

		frameBuffer[i] /= rpp;
	}

}

//------------------------------------------------------------------------------
/**
*/
bool
Raytracer::raycast(size_t ray_index)
{
	bool isHit = false;
	auto& result = results[ray_index];

	const auto& world = this->objects;

	for (size_t i = 0; i < world.size(); ++i)
	{
		if (world[i]->Intersect(result, rays[ray_index], result.t))
		{
			result.object = world[i];
			isHit = true;
		}
	}
	
	return isHit;
}

//------------------------------------------------------------------------------
/**
*/
Color
Raytracer::skybox(vec3 direction) const
{
	float t = 0.5 * (direction.y + 1.0);
	vec3 vec = vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
    	return {(float)vec.x, (float)vec.y, (float)vec.z};	
}

//------------------------------------------------------------------------------
/**
*/
void
Raytracer::clear()
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
Raytracer::update_matrices()
{
	mat4 inverseView = inverse(this->view); 
	mat4 basis = transpose(inverseView);
	this->frustum = basis;
}
