
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
// A cube has 6 faces 
// Each face has 2 triangles - 6 faces * 2 triangles == 12 triangles (nFaces)
// Each triangle has 3 vertices - 12 triangles * 3 vertices == 36 vertices (nVerts)

constexpr static int nVerts = sizeof(vertex_buffer_data) / sizeof(vertex_buffer_data[0]) / 3;
constexpr static int nFaces = nVerts / 3;


cube::cube()
{
    // Stuff all the vertices into the mesh vertex collection
    for (size_t idx = 0; idx < nVerts; idx += 3)
    {
        Vec3f v(vertex_buffer_data[idx], vertex_buffer_data[idx + 1], vertex_buffer_data[idx + 2]);

        addVertex(v);
    }

    // Now that we have vertices, we can construct faces
    // taking 3 vertices at a time.
    // We also construct the normals at the same time
    for (size_t fNum = 0; fNum < nFaces; fNum ++) 
    {
        int idx0 = (fNum * 3);
        int idx1 = (fNum * 3) + 1;
        int idx2 = (fNum * 3) + 2;

        Vec3i faceVerts(idx0, idx1, idx2);
            
        // we should probably take 
        // a normal at the same time
        // here we are only doing a face normal
        // not a normal per vertex
        Vec3f v1 = vert(idx1) - vert(idx0);
        Vec3f v2 = vert(idx2) - vert(idx0);
        Vec3f n = cross(v1, v2);
        size_t normIdx = addNormal(n);
        printf("normIdx: %d\n", normIdx);

        Vec3i tmpNorms(normIdx, normIdx, normIdx);


        // we should also fix the uv value
        Vec3i uv;
        
        addFace(faceVerts, tmpNorms, uv);
    }
}


