#ifndef XRNG_H_
#define XRNG_H_

#include <stdint.h>

#include <functional>

namespace xrng
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

		xoshiro128_plus(uint32_t seed=2243)
		{
			this->seed(seed);
			this->jump();
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

		void jump() 
		{
			static const uint32_t JUMP[] = { 0x8764000b, 0xf542d2d3, 0x6fa035c3, 0x77f2db5b };

			uint32_t s0 = 0;
			uint32_t s1 = 0;
			uint32_t s2 = 0;
			uint32_t s3 = 0;
			for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
			{
				for(int b = 0; b < 32; b++) 
				{
					if (JUMP[i] & UINT32_C(1) << b) 
					{
						s0 ^= s[0];
						s1 ^= s[1];
						s2 ^= s[2];
						s3 ^= s[3];
					}
					next();	
				}
			}

			s[0] = s0;
			s[1] = s1;
			s[2] = s2;
			s[3] = s3;
		}

		void seed(uint32_t seed)
		{
			// I'd love to see the face of the terrible nerds behind this generator
			// When they see what's happening here

			s[0] = seed;
			s[1] = (seed += 31 * std::hash<uint32_t>()(seed));
			s[2] = (seed += 31 * std::hash<uint32_t>()(seed));
			s[3] = (seed += 31 * std::hash<uint32_t>()(seed));

			this->jump();
		}

		float to_float(uint32_t x) 
		{
			const union { uint32_t i; float f; } u = { .i = UINT32_C(0x7F) << 23 | x >> 9 };
			return u.f - 1.0;
		}
		
		float fnext()
		{
			return to_float(next());
		}
	};
}

#endif
