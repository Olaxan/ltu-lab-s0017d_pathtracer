// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "shared.h"
#include "raytracer.h"

#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

struct ThreadData
{
	Raytracer* owner;
	size_t offset;
	size_t count;
	size_t data;
};

//------------------------------------------------------------------------------
/**
*/
Raytracer::Raytracer(const mat4& view, std::vector<Color>& frameBuffer, const TraceData& data) :
	rpp(data.rays_per_pixel),
	bounces(data.bounces),
	width(data.width),
	height(data.height),
	ray_count(width * height * rpp),
	max_threads(data.max_threads),
	view(view),
	frustum(get_frustum(view)),
	frameBuffer(frameBuffer)
{
	rays.reserve(ray_count);	
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

	std::vector<pthread_t> tids(max_threads);
	std::vector<ThreadData> data(max_threads);

	for (size_t i = 0; i < ray_count; i++)
	{
		int x = (i / rpp) % width;
		int y = (i / rpp) / width;

		float u = ((float(x + rng.fnext()) * (1.0f / this->width)) * 2.0f) - 1.0f;
		float v = ((float(y + rng.fnext()) * (1.0f / this->height)) * 2.0f) - 1.0f;

		vec3 direction(u, v, -1.0f);
		direction = transform(direction, this->frustum);
		
		rays[i] = Ray(cam_pos, direction);	
	}
	
	if (max_threads > 0)
	{

		size_t rays_per_thread = ray_count / max_threads;

		for (size_t t = 0; t < max_threads; t++)
		{
			data[t] = { this, t * rays_per_thread, rays_per_thread, bounces };

			int err = pthread_create(&tids[t], NULL, &trace_helper, &data[t]);

			if (err > 0)
				fprintf (stderr, "A thread error occured while tracing: %d\n", err);

		}

		for (size_t t = 0; t < max_threads; t++)
		{
			pthread_join(tids[t], NULL);
		}	
	}
	else
	{
		ThreadData data = { this, 0, ray_count, bounces };
		trace_helper(&data);
	}

	if (max_threads > 0)
	{
		size_t pixels_per_thread = frameBuffer.size() / max_threads;

		for (size_t t = 0; t < max_threads; t++)
		{
			data[t] = { this, t * pixels_per_thread, pixels_per_thread, 0 };

			int err = pthread_create(&tids[t], NULL, &render_helper, &data[t]);

			if (err > 0)
				fprintf(stderr, "A thread error occured while rendering: %d\n", err);
		}

		for (size_t t = 0; t < max_threads; t++)
		{
			pthread_join(tids[t], NULL);
		}	
	}
	else
	{
		ThreadData data = { this, 0, frameBuffer.size(), 0 };
		render_helper(&data);
	}

}

void*
Raytracer::trace_helper(void* params)
{
	ThreadData* data = (ThreadData*)params;
	Raytracer* owner = data->owner;
	
	size_t bounces = data->data;
	size_t count = data->count;
	
	HitResult res;
	int shadow_pass;
	for (size_t b = 0; b < bounces; b++)
	{
		shadow_pass = (b < (bounces - 1));

		for (size_t r = 0; r < count; r++)
		{
			size_t ray_index = r + data->offset;

			Ray& ray = owner->rays[ray_index];

			if (ray.f)
				continue;

			if (owner->raycast(ray_index, res))
			{
				ray.c *= res.object->GetColor() * shadow_pass;
				res.object->ScatterRay(ray, res.p, res.normal);
			}
			else
			{
				ray.f = true;
				ray.c *= owner->skybox(ray.m);
			}
		}
	}

	return EXIT_SUCCESS;	
}

void*
Raytracer::render_helper(void* params)
{
	ThreadData* data = (ThreadData*)params;
	Raytracer* owner = data->owner;

	auto& frameBuffer = owner->frameBuffer;
	size_t rpp = owner->rpp;

	for (size_t i = 0; i < data->count; i++)
	{
		size_t pixel_index = i + data->offset;	

		for (size_t j = 0; j < rpp; j++)
		{
			frameBuffer[pixel_index] += owner->rays[pixel_index * rpp + j].c;
		}

		frameBuffer[pixel_index] /= rpp;
	}

	return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
/**
*/
bool
Raytracer::raycast(size_t ray_index, HitResult& result)
{
	bool isHit = false;
	const auto& world = this->objects;
	result.t = FLT_MAX;

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
Raytracer::skybox(const vec3& direction) const
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
	
}
