#pragma once

#include "model.hpp"
#include "renderer_gl.hpp"
#include "algebra.hpp"

extern mat<4, 4> ModelView; // "OpenGL" state matrices
extern mat<4, 4> Projection;

struct Shader : IShader {
    const Model& model;
    vec3 uniform_l;       // light direction in view coordinates
    mat<2, 3> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<3, 3> varying_nrm; // normal per vertex to be interpolated by FS
    mat<3, 3> view_tri;    // triangle in view coordinates

    Shader(const Model& m) : model(m) {
        uniform_l = proj<3>((ModelView * embed<4>(light_dir, 0.))).normalize(); // transform the light vector to view coordinates
    }

    virtual void vertex(const int iface, const int nthvert, vec4& gl_Position) {
        varying_uv.set_col(nthvert, model.uv(iface, nthvert));
        varying_nrm.set_col(nthvert, proj<3>((ModelView).invert_transpose() * embed<4>(model.normal(iface, nthvert), 0.)));
        gl_Position = ModelView * embed<4>(model.vert(iface, nthvert));
        view_tri.set_col(nthvert, proj<3>(gl_Position));
        gl_Position = Projection * gl_Position;
    }

    virtual bool fragment(const vec3 bar, TGAColor& gl_FragColor) {
        vec3 bn = (varying_nrm * bar).normalize(); // per-vertex normal interpolation
        vec2 uv = varying_uv * bar; // tex coord interpolation

        // for the math refer to the tangent space normal mapping lecture
        // https://github.com/ssloy/tinyrenderer/wiki/Lesson-6bis-tangent-space-normal-mapping
        mat<3, 3> AI = mat<3, 3>{ {view_tri.col(1) - view_tri.col(0), view_tri.col(2) - view_tri.col(0), bn} }.invert();
        vec3 i = AI * vec3(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
        vec3 j = AI * vec3(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);
        mat<3, 3> B = mat<3, 3>{ {i.normalize(), j.normalize(), bn} }.transpose();

        vec3 n = (B * model.normal(uv)).normalize(); // transform the normal from the texture to the tangent space
        double diff = std::max(0., n * uniform_l); // diffuse light intensity
        vec3 r = (n * (n * uniform_l) * 2 - uniform_l).normalize(); // reflected light direction, specular mapping is described here: https://github.com/ssloy/tinyrenderer/wiki/Lesson-6-Shaders-for-the-software-renderer
        double spec = std::pow(std::max(-r.z, 0.), 5 + sample2D(model.specular(), uv)[0]); // specular intensity, note that the camera lies on the z-axis (in view), therefore simple -r.z

        TGAColor c = sample2D(model.diffuse(), uv);
        for (int i : {0, 1, 2})
            gl_FragColor[i] = std::min<int>(10 + c[i] * (diff + spec), 255); // (a bit of ambient light, diff + spec), clamp the result

        return false; // the pixel is not discarded
    }
};

