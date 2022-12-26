#pragma once

#include "definitions.h"
#include <cstdint>

namespace ndt {
// implement ring buffer using queue
// https://en.wikipedia.org/wiki/Circular_buffer

struct circular_buff_t
{
	uint8_t* fBuffer;
	size_t fSize;
	size_t fHead;
	size_t fTail;
};
}


namespace ndt {
	static INLINE void circular_buff_reset(circular_buff_t& cb, uint8_t* buff, size_t sz)
	{
		cb.fBuffer = buff;
		cb.fSize = sz;
		cb.fHead = 0;
		cb.fTail = 0;
	}

	static INLINE bool circular_buff_empty(circular_buff_t& cb)
	{
		return cb.fHead == cb.fTail;
	}

	static INLINE bool circular_buff_full(circular_buff_t& cb)
	{
		return ((cb.fHead + 1) % cb.fSize) == cb.fTail;
	}

	static INLINE size_t circular_buff_capacity(circular_buff_t& cb)
	{
		return cb.fSize;
	}

	static INLINE size_t circular_buff_size(circular_buff_t& cb)
	{
		return (cb.fSize + cb.fHead - cb.fTail) % cb.fSize;
	}

	static INLINE int circular_buff_put(circular_buff_t& cb, uint8_t data) {
		int r = -1;

		if (!circular_buff_full(cb))
		{
			cb.fBuffer[cb.fHead] = data;
			cb.fHead = (cb.fHead + 1) % cb.fSize;
			r = 0;
		}
		return r;
	}

	static INLINE int circular_buff_get(circular_buff_t& cb, uint8_t& data)
	{
		int r = -1;
		if (!circular_buff_empty(cb))
		{
			data = cb.fBuffer[cb.fTail];
			cb.fTail = (cb.fTail + 1) % cb.fSize;
			r = 0;
		}
		return r;
	}
}
