#pragma once

#include <vector>

#include <float.h>

#include "vec3.h"
#include "mat4.h"
#include "ray.h"
#include "object.h"

struct TraceData
{
	size_t width;
	size_t height;
	size_t rays_per_pixel;
	size_t bounces;
	size_t max_threads;
};

//------------------------------------------------------------------------------
/**
*/
class Raytracer
{
public:
	Raytracer(const mat4& view, std::vector<Color>& frameBuffer, const TraceData& data);
	~Raytracer();

	// start raytracing!
	void trace();

	// single raycast, find object
	bool raycast(size_t ray_index, HitResult& result);

	static void* trace_helper(void*);
	static void* render_helper(void*); 

	// get the color of the skybox in a direction
	Color skybox(const vec3& direction) const;

	// set camera matrix
	void set_view_matrix(const mat4& val);

	// clear screen
	void clear();

	// add object to scene
	void add_object(Object* obj);

	// update matrices. Called automatically after setting view matrix
	void update_matrices(); 		

private:

	// rays per pixel
	const unsigned rpp;
	// max number of bounces before termination
	const unsigned bounces = 5;
	// width of framebuffer
	const unsigned width;
	// height of framebuffer
	const unsigned height;
	// ray count for convenience
	const unsigned ray_count;
	// max allowed concurrent threads
	const unsigned max_threads;
	
	const vec3 lowerLeftCorner = { -2.0, -1.0, -1.0 };
	const vec3 horizontal = { 4.0, 0.0, 0.0 };
	const vec3 vertical = { 0.0, 2.0, 0.0 };
	const vec3 origin = { 0.0, 2.0, 10.0f };

	// view matrix
	const mat4 view;
	// Go from canonical to view frustum
	const mat4 frustum;

	std::vector<Color>& frameBuffer;
	std::vector<Object*> objects;
	std::vector<Ray> rays;
};

inline void Raytracer::add_object(Object* o)
{
	this->objects.push_back(o);
}
