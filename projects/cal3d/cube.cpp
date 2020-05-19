
#include "cube.h"

static float vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
};

// Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices

cube::cube()
{
        int nVerts = sizeof(vertex_buffer_data) / sizeof(float) / 3;
        for (size_t idx = 0; idx < nVerts; idx += 3)
        {
            Vec3f v(vertex_buffer_data[idx], vertex_buffer_data[idx + 1], vertex_buffer_data[idx + 2]);

            addVert(v);
        }

        // add the faces
        for (size_t fNum = 0; fNum < nverts()/3; fNum++) {
            std::vector<Vec3i> f;
            int idx0 = (fNum * 3);
            int idx1 = (fNum * 3) + 1;
            int idx2 = (fNum * 3) + 2;

            Vec3i tmp(idx0, idx1, idx2);
            f.push_back(tmp);
            
            // we should probably take 
            // a normal at the same time
            // here we are only doing a face normal
            // not a normal per vertex
            Vec3f v1 = vert(idx1) - vert(idx0);
            Vec3f v2 = vert(idx2) - vert(idx0);
            Vec3f n = cross(v1, v2);
            addNormal(n);

            // we should also fix the uv value
            addFace(f);
        }
    }
};

