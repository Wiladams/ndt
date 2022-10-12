#pragma once

#include <stdint.h>

namespace ndt
{
	/*
	 * adr must be aligned on an 'int'
	 */
	static void memset_s(void* adr, int32_t val, int32_t count) {
		int32_t i, n, v;
		uint32_t* p;
		uint16_t* q;

		p = static_cast<uint32_t*>(adr);
		v = val | (val << 16);

		n = count >> 3;
		for (i = 0; i < n; i++) {
			p[0] = v;
			p[1] = v;
			p[2] = v;
			p[3] = v;
			p += 4;
		}

		q = (uint16_t*)p;
		n = count & 7;
		for (i = 0; i < n; i++)
			*q++ = val;
	}


	// Used to rapidly copy 32 bit values 
	static void memset_l(void* adr, int32_t val, size_t count) 
	{
		int32_t i, n, v;
		uint32_t* p;
		p = static_cast<uint32_t *>(adr);
		v = val;

		// Do 4 at a time
		n = count >> 2;
		for (i = 0; i < n; i++) 
		{
			p[0] = v;
			p[1] = v;
			p[2] = v;
			p[3] = v;
			p += 4;
		}

		// Copy the last remaining values
		n = count & 3;
		for (i = 0; i < n; i++)
			*p++ = val;
	}

}
