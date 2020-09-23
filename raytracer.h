#pragma once

#include <vector>

#include <float.h>

#include "vec3.h"
#include "mat4.h"
#include "ray.h"
#include "object.h"

//------------------------------------------------------------------------------
/**
*/
class Raytracer
{
public:
	Raytracer(unsigned w, unsigned h, std::vector<Color>& frameBuffer, unsigned rpp, unsigned bounces);
	~Raytracer();

	// start raytracing!
	void Raytrace();

	// trace a path and return intersection color
	// n is bounce depth
	Color trace_step(size_t ray_index);	

	// single raycast, find object
	bool raycast(size_t ray_index, HitResult& result);

	// get the color of the skybox in a direction
	Color Skybox(vec3 direction) const;

	// set camera matrix
	void SetViewMatrix(mat4 val);

	// clear screen
	void Clear();

	// add object to scene
	void AddObject(Object* obj);

	// update matrices. Called automatically after setting view matrix
	void UpdateMatrices(); 	
	
	// rays per pixel
	unsigned rpp;
	// max number of bounces before termination
	unsigned bounces = 5;

	// width of framebuffer
	const unsigned width;
	// height of framebuffer
	const unsigned height;
	
	const vec3 lowerLeftCorner = { -2.0, -1.0, -1.0 };
	const vec3 horizontal = { 4.0, 0.0, 0.0 };
	const vec3 vertical = { 0.0, 2.0, 0.0 };
	const vec3 origin = { 0.0, 2.0, 10.0f };

	// view matrix
	mat4 view;
	// Go from canonical to view frustum
	mat4 frustum;

private:

	std::vector<Color>& frameBuffer;
	std::vector<Object*> objects;
	std::vector<Ray> rays;
	//std::vector<HitResult> results;
};

inline void Raytracer::AddObject(Object* o)
{
	this->objects.push_back(o);
}

inline void Raytracer::SetViewMatrix(mat4 val)
{
	this->view = val;
	this->UpdateMatrices();
}
