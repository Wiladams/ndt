
#include <algorithm>
#include <limits>
#include "dproc_gl.h"

Matrix ogl_lookat(Vec3f &eye, Vec3f &center, Vec3f &up) 
{
	Vec3f z = (eye - center).normalize();
	Vec3f x = cross(up, z).normalize();
	Vec3f y = cross(z, x).normalize();
	Matrix Minv = Matrix::identity();
	Matrix Tr = Matrix::identity();
	for (int i = 0; i<3; i++) 
	{
		Minv[0][i] = x[i];
		Minv[1][i] = y[i];
		Minv[2][i] = z[i];
		Tr[i][3] = -center[i];
	}

	Matrix m = Minv*Tr;
	
	return m;
}


Matrix ogl_viewport(const int x, const int y, const int w, const int h)
{
	Matrix res = Matrix::identity();


	res[0][3] = x + w / 2.f;
	res[1][3] = y + h / 2.f;
	res[2][3] = 1.f;
	res[0][0] = w / 2.f;
	res[1][1] = h / 2.f;
	res[2][2] = 0;

	return res;
}

Matrix ogl_projection(float coeff)
{
	Matrix res = Matrix::identity();
	res[3][2] = coeff;

	return res;
}


void ogl_clearzbuffer(float *zbuffer, const size_t width, const size_t height)
{
	// Zero out the zbuffer in case the scene has changed
	for (int i = width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());
}

Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P)
{
	Vec3f s[2];
	for (int i = 2; i--; )
	{
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 1e-2) {
		// dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	}

	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizer
}

void ogl_triangle(Matrix &Viewport, mat<4, 3, float> &clipc, IShader &shader, BLImage &img, float *zbuffer)
{
	BLImageData imageData;
	img.getData(&imageData);
	
	// quick sanity check
	if (nullptr == imageData.pixelData)
		return;

	mat<3, 4, float> pts = (Viewport*clipc).transpose(); // transposed to ease access to each of the points
	mat<3, 2, float> pts2;
	for (int i = 0; i<3; i++) 
		pts2[i] = proj<2>(pts[i] / pts[i][3]);

	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(img.width() - 1, img.height() - 1);
	
	for (int i = 0; i<3; i++) {
		for (int j = 0; j<2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts2[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2[i][j]));
		}
	}

	Vec2i P;
	BLRgba32 fragcolor;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts2[0], pts2[1], pts2[2], P);
			Vec3f bc_clip = Vec3f(bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3]);
			bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
			float frag_depth = clipc[2] * bc_clip;
			
			if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0 || zbuffer[P.x + (P.y*img.width())]>frag_depth)
				continue;

			bool discard = shader.fragment(bc_clip, fragcolor);

			if (!discard) {
				zbuffer[P.x + (P.y*img.width())] = frag_depth;

				// image.set(Pix.x, Pix.y, color)
				((BLRgba32*)(imageData.pixelData))[P.x + (P.y * img.width())] = fragcolor;
			}
		}
	}
}
