#include "apphost.h"

#include <limits>
#include <future>

#include "model.hpp"
#include "renderer_gl.hpp"
#include "Graphics.h"

constexpr int width  = 800; // output image size
constexpr int height = 800;

const vec3f light_dir(1,1,1); // light source
const vec3f       eye(1,1,3); // camera position
const vec3f    center(0,0,0); // camera direction
const vec3f        up(0,1,0); // camera up vector

SceneRenderer Renderer{};


struct Shader : IShader {
    const Model& model;
    vec3f uniform_l;       // light direction in view coordinates
    mat<2, 3> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<3, 3> varying_nrm; // normal per vertex to be interpolated by FS
    mat<3, 3> view_tri;    // triangle in view coordinates

    Shader(const Model& m) : model(m) {
        uniform_l = proj<3>((Renderer.ModelView * embed<4>(light_dir, 0.))).normalize(); // transform the light vector to view coordinates
    }

    virtual void vertex(const int iface, const int nthvert, vec4f& gl_Position) {
        varying_uv.set_col(nthvert, model.uv(iface, nthvert));
        varying_nrm.set_col(nthvert, proj<3>((Renderer.ModelView).invert_transpose() * embed<4>(model.normal(iface, nthvert), 0.)));
        gl_Position = Renderer.ModelView * embed<4>(model.vert(iface, nthvert));
        view_tri.set_col(nthvert, proj<3>(gl_Position));
        gl_Position = Renderer.Projection * gl_Position;
    }

    virtual bool fragment(const vec3f bar, maths::vec4b& gl_FragColor) override{
        vec3f bn = (varying_nrm * bar).normalize(); // per-vertex normal interpolation
        vec2f uv = varying_uv * bar; // tex coord interpolation

        // for the math refer to the tangent space normal mapping lecture
        // https://github.com/ssloy/tinyrenderer/wiki/Lesson-6bis-tangent-space-normal-mapping
        mat<3, 3> AI = mat<3, 3>{ {view_tri.col(1) - view_tri.col(0), view_tri.col(2) - view_tri.col(0), bn} }.invert();
        vec3f i = AI * vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
        vec3f j = AI * vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);
        mat<3, 3> B = mat<3, 3>{ {i.normalize(), j.normalize(), bn} }.transpose();

        vec3f n = (B * model.normal(uv)).normalize(); // transform the normal from the texture to the tangent space
        double diff = std::max(0., dot(n , uniform_l)); // diffuse light intensity
        vec3f r = (n * (dot(n , uniform_l)) * 2 - uniform_l).normalize(); // reflected light direction, specular mapping is described here: https://github.com/ssloy/tinyrenderer/wiki/Lesson-6-Shaders-for-the-software-renderer
        double spec = std::pow(std::max(-r.z, 0.), 5 + sample2D(model.specular(), uv)[0]); // specular intensity, note that the camera lies on the z-axis (in view), therefore simple -r.z

        maths::vec4b c = sample2D(model.diffuse(), uv);
        for (int i : {0, 1, 2})
            gl_FragColor[i] = std::min<int>(10 + c[i] * (diff + spec), 255); // (a bit of ambient light, diff + spec), clamp the result

        return false; // the pixel is not discarded
    }
};

void drawHUD()
{
    screenRefresh();
}

void onLoad() 
{


    if (2>gargc) {
        std::cerr << "Usage: " << gargv[0] << " obj/model.obj" << std::endl;
        
        halt();

        return;
    }

    createAppWindow(width, height,"tinyrenderer");
    appFrameBuffer().setOrientation(PixelOrientation::BottomToTop);

    Renderer.setLookAt(eye, center, up);                            // build the ModelView matrix
    Renderer.setViewport(width/8, height/8, width*3/4, height*3/4); // build the Viewport matrix
    Renderer.setProjection((center- eye).norm());                    // build the Projection matrix
    std::vector<double> zbuffer(width*height, std::numeric_limits<double>::max());

    for (int m=1; m<gargc; m++) { // iterate through all input objects
        Model model(gargv[m]);
        Shader shader(model);
        for (int i=0; i<model.nfaces(); i++) { // for every triangle
            vec4f clip_vert[3]; // triangle coordinates (clip coordinates), written by VS, read by FS
            for (int j : {0,1,2})
                shader.vertex(i, j, clip_vert[j]); // call the vertex shader for each triangle vertex
            Renderer.triangle(clip_vert, shader, appFrameBuffer(), zbuffer); // actual rasterization routine call
        }
    }

    drawHUD();
}

