// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "shared.h"
#include "raytracer.h"

#include <cstdio>
#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

struct ThreadData
{
	Raytracer* owner;
	size_t data_offset;
	size_t width;
	size_t x_mod;
	size_t height;
	size_t y_offset;
};

#define KB(x)   ((size_t) (x) << 10)
#define MB(x)   ((size_t) (x) << 20)
#define GB(x)	((size_t) (x) << 30)

#define RAY_SIZE 64

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
	if (data.max_memory == 0)
	{
		passes = 1;
	}
	else
	{
		// If max memory is specified, do multiple passes over data.
		passes = fmax(1, (ray_count * RAY_SIZE) / GB(data.max_memory));
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Raytracer::trace()
{

	std::vector<pthread_t> tids(max_threads);
	std::vector<ThreadData> data(max_threads);

	size_t p_width = this->width / passes;
	size_t x_mod = this->width % passes;
	
	if (max_threads > 0)
	{
		size_t t_height = this->height / max_threads;
		size_t y_mod = this->height % max_threads;

		size_t t_rays = (p_width + x_mod) * (t_height + y_mod) * rpp;
		size_t t_rays_vector_size = max_threads * t_rays;

		origins.reserve(t_rays_vector_size);
		directions.reserve(t_rays_vector_size);
		colors.reserve(t_rays_vector_size);
		finished.reserve(t_rays_vector_size);

		for (size_t t = 0; t < max_threads; t++)
		{
			// Last thread gets some extra pixels to render in order to fill entire image.
			data[t] = { this, t * t_rays, p_width, x_mod, t_height + (t == max_threads - 1) * y_mod, t * t_height };

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
		ThreadData data = { this, 0, p_width, 0, height, 0 };
		trace_helper(&data);
	}
}

void*
Raytracer::trace_helper(void* params)
{
	ThreadData* data = (ThreadData*)params;
	Raytracer* owner = data->owner;

	vec3 cam_pos = get_position(owner->view);
	
	size_t& passes = owner->passes;
	size_t& bounces = owner->bounces;
	size_t& rpp = owner->rpp;

	size_t& data_offset = data->data_offset;
	size_t& width = data->width;
	size_t& x_mod = data->x_mod;
	size_t& height = data->height;
	size_t& y_offset = data->y_offset;
	
	auto& origin = owner->origins;
	auto& direction = owner->directions;
	auto& color = owner->colors;
	auto& finished = owner->finished;

	auto& frameBuffer = owner->frameBuffer;
	
	HitResult res;

	for (size_t p = 0; p < passes; p++)
	{	

		size_t x_offset = p * width;

		// Expand width if this is the last pass in order to fill entire image.
		width = width + (p == passes - 1) * x_mod;
		size_t ray_count = width * height * rpp;

		for (size_t i = 0; i < ray_count; i++)
		{
			int x = ((i / rpp) % width) + x_offset;
			int y = ((i / rpp) / width) + y_offset;

			float u = ((float(x + rng.fnext()) * (1.0f / owner->width)) * 2.0f) - 1.0f;
			float v = ((float(y + rng.fnext()) * (1.0f / owner->height)) * 2.0f) - 1.0f;

			vec3 dir(u, v, -1.0f);
			dir = transform(dir, owner->frustum);
			
			origin[i + data_offset] = cam_pos;
			direction[i + data_offset] = dir;
			color[i + data_offset] = { 1, 1, 1 };
			finished[i + data_offset] = false;	
		}

		for (size_t b = 0; b < bounces; b++)
		{
			int shadow_pass = (b < (bounces - 1));

			for (size_t r = 0; r < ray_count; r++)
			{
			
				size_t ray_index = r + data_offset;

				res.dist = FLT_MAX;
				res.shape = Shapes::None;
				
				if (finished[ray_index])
					continue;

				owner->raycast_spheres(origin[ray_index], direction[ray_index], res);

				// Add raycast functions for additional shapes here

				switch (res.shape)
				{
					case Shapes::None:
					{
						finished[ray_index] = true;
						color[ray_index] *= owner->skybox(direction[ray_index]);
						break;
					}
					case Shapes::Sphere:
					{
						Sphere& obj = owner->spheres[res.index];
						Material& mat = owner->materials[obj.material_index];

						color[ray_index] *= mat.color * shadow_pass;
						BSDF(mat, origin[ray_index], direction[ray_index], res.p, res.normal);
						break;
					}
					default:
						fprintf(stderr, "Undefined shape type %u hit!\n", res.shape);
					break;
				}
			}
		}

		for (size_t i = 0; i < ray_count; i += rpp)
		{

			int x = ((i / rpp) % width) + x_offset;
			int y = ((i / rpp) / width) + y_offset;

			size_t pixel_index = x + owner->width * y;

			for (size_t j = 0; j < rpp; j++)
			{
				frameBuffer[pixel_index] += color[i + j + data_offset];
			}

			frameBuffer[pixel_index] /= rpp;
		}
	}	

	return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
/**
*/
bool
Raytracer::raycast_spheres(const vec3& point, const vec3& dir, HitResult& result)
{
	bool isHit = false;

	for (size_t i = 0; i < spheres.size(); ++i)
	{
		if (spheres[i].intersect(point, dir, result.dist, result))
		{
			isHit = true;
			result.index = i;
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

