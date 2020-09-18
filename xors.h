#include <stdint.h>

namespace xorshift
{

	static inline uint32_t rotl(const uint32_t x, int k) 
	{
		return (x << k) | (x >> (32 - k));
	}

	static uint32_t s[4];

	uint32_t next() 
	{
		const uint32_t result = s[0] + s[3];

		const uint32_t t = s[1] << 9;

		s[2] ^= s[0];
		s[3] ^= s[1];
		s[1] ^= s[2];
		s[0] ^= s[3];

		s[2] ^= t;

		s[3] = rotl(s[3], 11);

		return result;
	}

	float fnext()
	{
		return (float)next();
	}

} 
