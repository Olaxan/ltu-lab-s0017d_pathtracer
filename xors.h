#include <stdint.h>

#include <functional>

namespace prng
{

	class xoshiro128_plus
	{
	private:

		static inline uint32_t rotl(const uint32_t x, int k) 
		{
			return (x << k) | (x >> (32 - k));
		}

		uint32_t s[4];

	public:

		xoshiro128_plus(uint32_t seed)
		{
			this->seed(seed);
		}

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

		void seed(uint32_t seed)
		{
			// I'd love to see the face of the terrible nerds behind this generator
			// When they see what's happening here

			s[0] = seed;
			s[1] = (seed += 31 * std::hash<uint32_t>()(seed));
			s[2] = (seed += 31 * std::hash<uint32_t>()(seed));
			s[3] = (seed += 31 * std::hash<uint32_t>()(seed));
		}

		float fnext()
		{
			return to_float(next());
		}

		float to_float(uint32_t x) 
		{
			const union { uint32_t i; float f; } u = { .i = UINT32_C(0x7F) << 23 | x >> 9 };
			return u.f - 1.0;
		}
	};
} 
