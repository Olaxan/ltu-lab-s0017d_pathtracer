#pragma once

#include <vector>

#include <float.h>

#include "vec3.h"
#include "mat4.h"
#include "object.h"
#include "sphere.h"

#include <cstdio>

struct TraceData
{
	size_t width;
	size_t height;
	size_t rays_per_pixel;
	size_t bounces;
	size_t max_threads;
	size_t max_memory;
};

//------------------------------------------------------------------------------
/**
*/
class Raytracer
{
public:
	Raytracer(const mat4& view, std::vector<Color>& frameBuffer, const TraceData& data);
	~Raytracer() = default;

	// start raytracing!
	void trace();

	static void* trace_helper(void*);

	// get the color of the skybox in a direction
	Color skybox(const vec3& direction) const;

	// set camera matrix
	void set_view_matrix(const mat4& val);

	// clear screen
	void clear();

	size_t add_material(Material mat);

	// add object to scene
	size_t add_sphere(const vec3& center, float radius, size_t material);
	// single raycast, find object
	bool raycast_spheres(const vec3& point, const vec3& dir, HitResult& result);

	// update matrices. Called automatically after setting view matrix
	void update_matrices(); 		

private:

	// rays per pixel
	const size_t rpp;
	// max number of bounces before termination
	const size_t bounces = 5;
	// width of framebuffer
	const size_t width;
	// height of framebuffer
	const size_t height;
	// ray count for convenience
	const size_t ray_count;
	// max allowed concurrent threads
	const size_t max_threads;
	// number of render passes to perform
	size_t passes;
	
	const vec3 lowerLeftCorner = { -2.0, -1.0, -1.0 };
	const vec3 horizontal = { 4.0, 0.0, 0.0 };
	const vec3 vertical = { 0.0, 2.0, 0.0 };
	const vec3 origin = { 0.0, 2.0, 10.0f };

	// view matrix
	const mat4 view;
	// Go from canonical to view frustum
	const mat4 frustum;

	std::vector<Color>& frameBuffer;
	std::vector<Material> materials;

	// Ray data
	std::vector<vec3> origins;
	std::vector<vec3> directions;
	std::vector<Color> colors;
	std::vector<bool> finished;
	
	// Shape vectors
	std::vector<Sphere> spheres;
};

inline size_t Raytracer::add_material(Material mat)
{
	this->materials.push_back(mat);
	return materials.size() - 1;
}

inline size_t Raytracer::add_sphere(const vec3& center, float radius, size_t material)
{
	this->spheres.push_back(Sphere(radius, center, material));
	return spheres.size() - 1;
}
