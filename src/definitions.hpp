// Some well known types
typedef float	float32;
typedef double	float64;
typedef float32 REAL;

#define U64x(hi, lo)	(((uint64_t)0x##hi << 32) + (uint64_t)0x##lo)
