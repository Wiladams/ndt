#include <limits>

#include "renderer_gl.hpp"
#include "pixelaccessor.h"

mat4f ModelView;
mat4f Viewport;
mat4f Projection;

void viewport(const int x, const int y, const int w, const int h) {
    Viewport = {{{w/2., 0, 0, x+w/2.}, {0, h/2., 0, y+h/2.}, {0,0,1,0}, {0,0,0,1}}};
}

void projection(const double f) { // check https://en.wikipedia.org/wiki/Camera_matrix
    Projection = {{{1,0,0,0}, {0,-1,0,0}, {0,0,1,0}, {0,0,-1/f,0}}}; // P[1,1] = -1; does vertical flip
}

void lookat(const vec3f eye, const vec3f center, const vec3f up) { // check https://github.com/ssloy/tinyrenderer/wiki/Lesson-5-Moving-the-camera
    vec3f z = (center-eye).normalize();
    vec3f x =  cross(up,z).normalize();
    vec3f y =  cross(z, x).normalize();
    mat4f Minv = {{{x.x,x.y,x.z,0.0},   {y.x,y.y,y.z,0},   {z.x,z.y,z.z,0},   {0,0,0,1}}};
    mat4f Tr   = {{{1,0,0,-eye.x}, {0,1,0,-eye.y}, {0,0,1,-eye.z}, {0,0,0,1}}};
    ModelView = Minv*Tr;
}

vec3f barycentric(const vec2f tri[3], const vec2f P) {
    mat<3,3> ABC = {{embed<3>(tri[0]), embed<3>(tri[1]), embed<3>(tri[2])}};
    if (ABC.det()<1e-3) return vec3f(-1,1,1); // for a degenerate triangle generate negative coordinates, it will be thrown away by the rasterizator
    return ABC.invert_transpose() * embed<3>(P);
}

void triangle(const vec4f clip_verts[3], IShader &shader, PixelAccessor<maths::vec4b> &image, std::vector<double> &zbuffer) {
    vec4f pts[3]  = { Viewport*clip_verts[0],    Viewport*clip_verts[1],    Viewport*clip_verts[2]    };  // triangle screen coordinates before persp. division
    vec2f pts2[3] = { proj<2>(pts[0]/pts[0][3]), proj<2>(pts[1]/pts[1][3]), proj<2>(pts[2]/pts[2][3]) };  // triangle screen coordinates after  perps. division

    vec2f bboxmin( std::numeric_limits<double>::max(),  std::numeric_limits<double>::max());
    vec2f bboxmax(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
    vec2f clamp(image.width()-1, image.height()-1);
    for (int i=0; i<3; i++)
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.,       std::min(bboxmin[j], pts2[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2[i][j]));
        }
#pragma omp parallel for
    for (int x=(int)bboxmin.x; x<=(int)bboxmax.x; x++) {
        for (int y=(int)bboxmin.y; y<=(int)bboxmax.y; y++) {
            vec3f bc_screen = barycentric(pts2, vec2f(x, y));
            vec3f bc_clip   = vec3f(bc_screen.x/pts[0][3], bc_screen.y/pts[1][3], bc_screen.z/pts[2][3]);
            bc_clip = bc_clip/(bc_clip.x+bc_clip.y+bc_clip.z); // check https://github.com/ssloy/tinyrenderer/wiki/Technical-difficulties-linear-interpolation-with-perspective-deformations
            double frag_depth = dot(vec3f(clip_verts[0][2], clip_verts[1][2], clip_verts[2][2]), bc_clip);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0 || frag_depth > zbuffer[x+y*image.width()]) continue;
            maths::vec4b color;
            if (shader.fragment(bc_clip, color)) continue; // fragment shader can discard current fragment
            zbuffer[x+y*image.width()] = frag_depth;
            maths::vec4b c{ { color[0], color[1], color[2], 255 } };
            image.setPixel(x, y, c);
        }
    }
}

