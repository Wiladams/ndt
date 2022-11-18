
#include "maths.hpp"
#include "pixelaccessor.h"

#include "tgaimage.hpp"
#include "algebra.hpp"

void viewport(const int x, const int y, const int w, const int h);
void projection(const double coeff=0); // coeff = -1/c
void lookat(const vec3f eye, const vec3f center, const vec3f up);

struct IShader {
    static maths::vec4b sample2D(const TGAImage &img, vec2f &uvf) {
        return img.getPixel(uvf[0] * img.width(), uvf[1] * img.height());
    }
    virtual bool fragment(const vec3f bar, maths::vec4b &color) = 0;
};

void triangle(const vec4f clip_verts[3], IShader &shader, PixelAccessor<maths::vec4b>&image, std::vector<double> &zbuffer);

