/*
    This is a good stress test for polygon drawing.
    In particular, it's interesting to watch memory
    consumption, as well as CPU usage.

    Up_Arrorw - Increase the number of iterations per frame by 1,000
    Down_Arrow - Decrease number of iterations per frame by 1,000
    ESC - Exit
*/
#include "blend2d.h"

#include <intrin.h>
#include <cstdint>
#include <cmath>


int ITERATIONS = 10000;
static const int width = 720;
static const int height = 400;


#define U64x(hi, lo)	(((uint64_t)0x##hi << 32) + (uint64_t)0x##lo)

// union used for getting bit patterns
// for doubles
typedef union {
    uint64_t u64;
    double d;
} U64double;

struct TausPRNG {
private:
    uint64_t gen[4];    // state of 4 generators
    int valid;          // State is valid

private:
    #define TW223_GEN(i, k, q, s) \
    z = this->gen[i]; \
    z = (((z<<q)^z) >> (k-s)) ^ ((z&((uint64_t)(int64_t)-1 << (64-k)))<<s); \
    r ^= z; this->gen[i] = z;

    uint64_t step()
    {
        uint64_t z, r = 0;
        TW223_GEN(0, 63, 31, 18)
        TW223_GEN(1, 58, 19, 28)
        TW223_GEN(2, 55, 24, 7)
        TW223_GEN(3, 47, 21, 8)

        return (r & U64x(000fffff,ffffffff)) | U64x(3ff00000,00000000);
    }


public:
    // Constructor with a seed
    TausPRNG(double d)
    {
        uint32_t r = 0x11090601;  // 64-k[i] as four 8 bit constants.
        int i;
        for (i = 0; i < 4; i++) {
            U64double u;
            uint32_t m = 1u << (r&255);
            r >>= 8;
            u.d = d = d * 3.14159265358979323846 + 2.7182818284590452354;
            if (u.u64 < m) // Ensure k[i] MSB of gen[i] are non-zero.
                u.u64 += m;  
            
            gen[i] = u.u64;
        }
        valid = 1;
        for (i = 0; i < 10; i++)
            step();
    }

    // Return a double value between 0..1, inclusive
    double next()
    {
        U64double u;
        double d;
        u.u64 = step();
        d = u.d - 1.0;

        return d;
    }

    // return an integer in the range [1..high]
    int next(int high)
    {
        double d = next();
        d = floor(d*high) + 1.0;
        return (int)d;
    }

    // return a value in the range
    // between low and high, inclusive
    double next(double low, double high)
    {
        double d = next();
        d = (d*(high-low+1.0)) + low;
        return d;
    }
};

TausPRNG mRandomNumberGenerator(5);

inline double random()
{
    return mRandomNumberGenerator.next();
}

inline double random(double high)
{
    return mRandomNumberGenerator.next(0, high);
}

inline double random(double low, double high)
{
    return mRandomNumberGenerator.next(low, high);
}

double lerp(double low, double high, double x)
{
    return low + x*(high-low);
}

BLRgba32 lerpColor(const BLRgba32 &from, const BLRgba32 &to, double f)
{
    uint32_t r = (uint32_t)lerp(from.r, to.r, f);
    uint32_t g = (uint32_t)lerp(from.g, to.g, f);
    uint32_t b = (uint32_t)lerp(from.b, to.b, f);
    uint32_t a = (uint32_t)lerp(from.a, to.a, f);

    return BLRgba32(r, g, b, a);
}

void quad(BLContext &ctx, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    BLPoint pts[] = { {x1,y1},{x2,y2},{x3,y3},{x4,y4} };
    ctx.fillPolygon(pts, 4);
}

void draw(BLContext &ctx)
{
    // either one of these two
    //background(255);
    ctx.clearAll();

    BLRgba32 from(255, 0, 0, (int)(0.2 * 255));
    BLRgba32 to(0, 0, 255, (int)(0.2 * 255));
    BLRgba32 c1 = lerpColor(from, to, 0.33);
    BLRgba32 c2 = lerpColor(from, to, 0.66);

    for (int i = 1; i <= ITERATIONS; i++) {
        ctx.setFillStyle(from);

        quad(ctx,
            random(-40, 220), random(height),
            random(-40, 220), random(height),
            random(-40, 220), random(height),
            random(-40, 220), random(height)
        );


        ctx.setFillStyle(c1);
        quad(ctx,
            random(140, 380), random(height),
            random(140, 380), random(height),
            random(140, 380), random(height),
            random(140, 380), random(height)
        );

        ctx.setFillStyle(c2);
        quad(ctx,
            random(320, 580), random(height),
            random(320, 580), random(height),
            random(320, 580), random(height),
            random(320, 580), random(height)
        );

        ctx.setFillStyle(to);
        quad(ctx,
            random(500, 760), random(height),
            random(500, 760), random(height),
            random(500, 760), random(height),
            random(500, 760), random(height)
        );

    }

}


int main()
{
    BLImage img(720, 400, BL_FORMAT_PRGB32);
    BLContext ctx(img);

    while (true)
    {
        // do the drawing
        draw(ctx);
        
        // do a little sync after each frame
        ctx.flush(BL_CONTEXT_FLUSH_SYNC);
    }

    return 0;
}