#pragma once

#include "vec3.h"

union Color
{
	Color() 
		: rgb() {}

	Color(float r, float g, float b)
		: r(r), g(g), b(b) {}

	Color(vec3 rgb)
		: rgb(rgb) {}

	vec3 rgb;

	Color(const Color& other)
	{
		this->rgb = other.rgb;
	}

	struct
	{
		float r = 0;
		float g = 0;
		float b = 0;
	};

	void operator += (Color const& rhs)
	{
		this->r += rhs.r;
		this->g += rhs.g;
		this->b += rhs.b;
	}

	Color operator + (Color const& rhs)
	{
		return Color(this->rgb + rhs.rgb);
	}

	Color operator * (Color const& rhs)
	{
		return Color(this->rgb * rhs.rgb);
	}

	~Color() {}
};
