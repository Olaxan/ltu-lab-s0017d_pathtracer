#pragma once

struct Color
{
	float r = 0;
	float g = 0;
	float b = 0;

	void operator += (Color const& rhs)
	{
		this->r += rhs.r;
		this->g += rhs.g;
		this->b += rhs.b;
	}

	void operator *= (Color const& rhs)
	{
		this->r *= rhs.r;
		this->g *= rhs.g;
		this->b *= rhs.b;	
	}

	void operator /= (int rhs)
	{
		this->r /= rhs;
		this->g /= rhs;
		this->b /= rhs;
	}

	Color operator + (Color const& rhs) const
	{
		return 
		{
			this->r + rhs.r,
			this->g + rhs.g,
			this->b + rhs.b
		};
	}

	Color operator * (Color const& rhs) const
	{
		return 
		{
			this->r * rhs.r,
			this->g * rhs.g,
			this->b * rhs.b
		};
	}

	Color operator * (const float& rhs) const
	{
		return 
		{
			this->r * rhs,
			this->g * rhs,
			this->b * rhs
		};
	}
};
