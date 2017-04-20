#pragma once
#include <stdint.h>

namespace ss{

class xorshift32{
private:
	uint32_t	y;

public:
	xorshift32(){}
	~xorshift32(){}

	void	init_genrand(unsigned long seed) {y=seed;}

	uint32_t genrand_uint32(){
		y = y ^ ( y << 13 );
		y = y ^ ( y >> 17 );
		y = y ^ ( y << 15 );

		return y;
	}

	float genrand_float32() {
		uint32_t v = genrand_uint32();
		uint32_t res = (v >> 9) | 0x3f800000;
		float	r = (*(float*)&res) - 1.0f;

		return r;
	}

};

} //namespace ss
