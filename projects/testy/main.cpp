#include <cstdio>

#include "bstream.h"

using namespace ndt;


void main()
{
	char buff[1024]{};

	auto cur = make_cursor_size(&buff[0], sizeof(buff));

	printf("Cursor Size: %zd\n", size(cur));

	put_u8(cur, 0x80);
	put_u16_le(cur, 0x0160);
	put_u32_le(cur, 0x03200000);
	uint64_t a64 = 0xfeedfacedeadfad0ull;
	put_u64_le(cur, a64);

	printf("tell after put: [15 ]%d\n", tell(cur));

	seek(cur, 0);
	uint8_t u8 = get_u8(cur);
	uint16_t u16 = get_u16_le(cur);
	uint32_t u32 = get_u32_le(cur);
	auto u64 = get_u64_le(cur);

	printf("tell after get: [15 ]%d\n", tell(cur));

	printf("u8: 0x%x\n", u8);
	printf("u16: 0x%x\n", u16);
	printf("u32: 0x%x\n", u32);
	printf("u64: 0x%0llx\n", u64);
}