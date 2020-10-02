#pragma once
/*
	Inspired from the book
	"Real-Time Animation Toolkit in C++"
	By: Rex E. Bradford

	This single header has enough stuff to do some
	simple 3D representations and animations.
*/
#include <memory>

#include "texture.h"
#include "canvas.h"

using AFix = float;

class AScene;
class AActor;

struct APolyVertex {
	AFix x;
	AFix y;
};

struct ATmapCoord {
	AFix u=0;
	AFix v=0;

	ATmapCoord() {}
	ATmapCoord(const AFix _u, const AFix _v) :u(_u), v(_v) {}

};

struct ARect {
	short left;
	short top;
	short right;
	short bottom;

	ARect(short l, short t, short r, short b)
		:left(l), top(t), right(r), bottom(b)
	{}

	ARect() {}

	// some computations
	short width() { return right - left; }
	short height() { return bottom - top; }
	bool isEmpty() { return (left > right) || (top >= bottom); }
	long area() { return (long)(right-left) * (long)(bottom-top); }

	// Tests for points and other rectangles

	// Compute some intersections
	ARect intersect(ARect& rect)
	{

	}

	// Calculate smallest extent that encloses both
	ARect smallestEnclosing(ARect& rect)
	{

	}

	int disjoint(ARect& rect, ARect* pResultArray)
	{

	}

};
// representation of coordinates in a 3D space
struct AVector {
	float x=0;
	float y=0;
	float z=0;

	// Constructors and assignment
	AVector() {}
	AVector(const float _x, const float _y, const float _z)
		:x(_x), y(_y), z(_z)
	{}

	AVector& operator=(const AVector& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	// Arithmetic
	AVector& operator+=(const AVector& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	AVector& operator-=(const AVector& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	friend AVector operator+(const AVector& lhs, const AVector& rhs)
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
	}

	friend AVector operator-(const AVector& lhs, const AVector& rhs)
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
	}

	// Scaling vectors
	AVector& operator*=(const float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	
	friend AVector operator*(const AVector & v, const float s)
	{
		return { v.x * s, v.y * s, v.z * s };
	}
	
	// Vector length and normalization
	float length()
	{
		return sqrt(x * x + y * y + z * z);
	}

	AVector& normalize()
	{
		float recipLength = 1.0f / length();
		*this *= recipLength;

		return *this;
	}

	// Calculate dot product of two vectors
	friend float dot(const AVector& a, const AVector& b)
	{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}

	// Compute cross product of two vectors and return result
	static AVector cross(const AVector& a, const AVector& b)
	{
		AVector r;
		r.x = (a.y * b.z) - (a.z * b.y);
		r.y = (a.z * b.x) - (a.x + b.z);
		r.z = (a.x * b.y) - (a.y * b.x);

		return r;
	}


	// Three vectors define a plane. 
	// Return a vector perpendicular to that plane

	static AVector normal(const AVector& a, const AVector& b, const AVector& c)
	{
		AVector norm = cross(b - a, b - c);
		norm.normalize();
		return norm;
	}
};

using ACoord3 = AVector;


// Matrix representation of 3D transformations
// Matrix layout
// 
// m[0]  m[1]  m[2]
// m[3]  m[4]  m[5]
// m[6]  m[7]  m[9]
//
// Maybe faster than m[3][3], maybe not
//
struct AMatrix 
{

	float m[9] = { 0 };

	AMatrix() {}
	AMatrix(
		const float m00, const float m01, const float m02,
		const float m10, const float m11, const float m12,
		const float m20, const float m21, const float m22)
	{
		m[0] = m00; m[1] = m01; m[2] = m02;
		m[3] = m10; m[4] = m11; m[5] = m12;
		m[6] = m20; m[7] = m21; m[8] = m22;
	}

	AMatrix& operator=(const AMatrix& a)
	{
		memcpy(m, a.m, sizeof(m));
		return *this;
	}

	AMatrix& setIdentity()
	{
		memset(m, 0, sizeof(m));
		m[0] = 1.0f;
		m[4] = 1.0f;
		m[8] = 1.0f;
		return *this;
	}

	// Matrix computations
	// set a scale on the matrix
	AMatrix& scaleXYZ(const AVector& v)
	{
		// scale first column by v.x
		m[0] *= v.x;
		m[3] *= v.x;
		m[6] *= v.x;

		// scale second column by v.y
		m[1] *= v.y;
		m[4] *= v.y;
		m[7] *= v.y;

		// scale third column by v.z
		m[2] *= v.z;
		m[5] *= v.z;
		m[8] *= v.z;

		return *this;
	}

	// multiply by matrix on the left
	// v' = v * m;
	friend AVector operator *(const AVector& v, const AMatrix& m)
	{
		return{
			(v.x * m.m[0]) + (v.y * m.m[3]) + (v.z * m.m[6]),
			(v.x * m.m[1]) + (v.y * m.m[4]) + (v.z * m.m[7]),
			(v.x * m.m[2]) + (v.y * m.m[5]) + (v.z * m.m[8])
		};
	}

	// multiply matrix by matrix
	// concatenate transformations
	friend AMatrix operator *(const AMatrix& a, const AMatrix& b)
	{
		AMatrix r;

		// an unrolled loop essentially
		r.m[0] = (a.m[0] * b.m[0]) + (a.m[1] * b.m[3]) + (a.m[2] * b.m[6]);
		r.m[1] = (a.m[0] * b.m[1]) + (a.m[1] * b.m[4]) + (a.m[2] * b.m[7]);
		r.m[2] = (a.m[0] * b.m[2]) + (a.m[1] * b.m[5]) + (a.m[2] * b.m[8]);

		r.m[3] = (a.m[3] * b.m[0]) + (a.m[4] * b.m[3]) + (a.m[5] * b.m[6]);
		r.m[4] = (a.m[3] * b.m[1]) + (a.m[4] * b.m[4]) + (a.m[5] * b.m[7]);
		r.m[5] = (a.m[3] * b.m[2]) + (a.m[4] * b.m[5]) + (a.m[5] * b.m[8]);

		r.m[6] = (a.m[6] * b.m[0]) + (a.m[7] * b.m[3]) + (a.m[8] * b.m[6]);
		r.m[7] = (a.m[6] * b.m[1]) + (a.m[7] * b.m[4]) + (a.m[8] * b.m[7]);
		r.m[8] = (a.m[6] * b.m[2]) + (a.m[7] * b.m[5]) + (a.m[8] * b.m[8]);

		return r;
	}
};

constexpr float FOV_90 = 1.5708f;	// 90 degree field of view

struct ACamera
{
	AVector location;	// camera location
	AVector direction;	// pitch, heading, bank
	AMatrix m;

	float fov=0;
	int viewportW=0;
	int viewportH=0;
	float viewportAspect=1.0;




	// Constructors
	ACamera() {}
	ACamera(AVector &camLoc, AVector &camDir, int gCanvasWidth, int gCanvasHeight, float _fov)
	{
		location = camLoc;
		direction = camDir;
		m.setIdentity();
		viewportW = gCanvasWidth;
		viewportH = gCanvasHeight;
		fov = _fov;

		calcViewMatrix();
	}

	// Attributes
	void setFov(const float _fov) { fov = _fov; calcViewMatrix(); }
	void setLocation(const AVector& v) { location = v; calcViewMatrix(); }
	void setDirection(const AVector& v) { direction = v; calcViewMatrix(); }
	void setLocAndDir(const AVector& _loc, const AVector& _dir) {
		location = _loc;
		direction = _dir;
		calcViewMatrix();
	}

	void setViewport(const int w, const int h, const float _fov, bool recalc = true)
	{
		viewportW = w;
		viewportH = h;
		fov = _fov;
		viewportAspect = float(h) / float(w);
		if (recalc) {
			calcViewMatrix();
		}
	}

	
	// Compute world-> viewport matrix
	void calcViewMatrix()
	{
		AVector rdir(-direction.x, -direction.y, -direction.z);

		m.m[0] = std::cos(rdir.y) * std::cos(rdir.z);
		m.m[1] = std::cos(rdir.y) * std::sin(rdir.z);
		m.m[2] = -std::sin(rdir.y);

		m.m[3] = (std::sin(rdir.x) * std::sin(rdir.y) * std::cos(rdir.z)) -
			(std::cos(rdir.x) * std::sin(rdir.z));
		m.m[4] = (std::sin(rdir.x) * std::sin(rdir.y) * std::sin(rdir.z)) +
			(std::cos(rdir.x) * std::cos(rdir.z));
		m.m[5] = std::sin(rdir.x) * std::cos(rdir.y);

		m.m[6] = (std::cos(rdir.x) * std::sin(rdir.y) * std::cos(rdir.z)) +
			(std::sin(rdir.x) * std::sin(rdir.z));
		m.m[7] = (std::cos(rdir.x) * sin(rdir.y) * std::sin(rdir.z)) -
			(std::sin(rdir.x) * cos(rdir.z));
		m.m[8] = std::cos(rdir.x) * std::cos(rdir.y);
	
		// scale by viewport
		AVector vscale(1.0f * viewportAspect, 1.0f, fov / FOV_90);
		m.scaleXYZ(vscale);
	}

	// Transform and project points
	void worldToView(AVector& ptsrc, AVector& ptdest)
	{
		// translate, then rotate/scale
		ptdest = (ptsrc - location) * m;
	}

	bool project(AVector& ptsrc, APolyVertex& pvdest)
	{
		// if behind camera, return false
		if (ptsrc.z >= 0) {
			return false;
		}

		pvdest.x = ((ptsrc.x / -ptsrc.z) * viewportW) + (viewportW / 2) + 0.5f;
		pvdest.y = viewportH - (((ptsrc.y / -ptsrc.z) * viewportH) + (viewportH / 2) + 0.5f);

		return true;
	}

	// check of transformed poly (screen coords) are facing forward
	static bool isFacingForward(const APolyVertex& v1, const APolyVertex& v2, const APolyVertex& v3)
	{
		// cros product of adjoining vectors tells you which 
		// direction the face is relative to the camera
		float x1 = v3.x - v2.x;
		float x2 = v1.x - v2.x;
		float y1 = v3.y - v2.y;
		float y2 = v1.y - v2.y;
		return ((x1 * y2 - y1 * y2) < 0);
	}
};




constexpr size_t MAX_POLY_VERTICES = 8;

enum {
	POLY_TYPE_FLAT = 0,
	POLY_TYPE_TMAP = 1
};

struct APolygon
{
	size_t numVerts;						// number of vertices
	APolyVertex* pVerts;					// point to array of vertices
	size_t vertIndex[MAX_POLY_VERTICES];	// indices into vertex array
	ATmapCoord* pTmapCoords;				// pointer to array of tmap coords, or NULL
	std::shared_ptr<Texture> tmap;

	size_t findTopmostPolyVertex() const
	{
		AFix ymin(32767);
		size_t vmin = 0;

		for (size_t i = 0; i < numVerts; i++) {
			if (pVerts[vertIndex[i]].y < ymin) {
				ymin = pVerts[vertIndex[i]].y;
				vmin = i;
			}
		}

		return vmin;	// return minimum vertex index;
	}
};

struct APolyDda
{
	short vertIndex=0;
	short vertNext=0;			// index of next vertex
	AFix x;					// current x value
	AFix dx;				// Stepping factor in x
	short ybeg=0;				// begin y-coord of span
	short yend=0;				// end y-coord of span
	ATmapCoord tc;			// texture map coordinate
	ATmapCoord tcDelta;		// x, y delta to texture coord

	APolyDda() {}
};



struct ACanvas : GCanvas
{
	std::shared_ptr<Texture> fTexture;
	BLBoxI clipRect;

	// Setup a dda for drawing
	static void setupPolyDda(const APolygon& poly, APolyDda& dda, const size_t ivert, const int dir)
	{
		dda.vertIndex = ivert;
		dda.vertNext = ivert + dir;
		if (dda.vertNext < 0)
			dda.vertNext = poly.numVerts - 1;
		else if (dda.vertNext == poly.numVerts)
			dda.vertNext = 0;

		// Set starting/ending ypos 
		dda.ybeg = dda.yend;
		dda.yend = round(poly.pVerts[poly.vertIndex[dda.vertNext]].y);
		dda.x = poly.pVerts[poly.vertIndex[dda.vertIndex]].x;

		// calculate fractional number of pixels to step in x (dda.dx)
		AFix xdelta = poly.pVerts[poly.vertIndex[dda.vertNext]].x - 
			poly.pVerts[poly.vertIndex[dda.vertIndex]].x;
		int ydelta = dda.yend - dda.ybeg;
		if (ydelta > 0)
			dda.dx = xdelta / ydelta;
		else
			dda.dx = 0;
	}

	static void setupTmapPolyDda(const APolygon& poly, APolyDda& dda, const size_t ivert, const int dir)
	{
		// Do normal poly scanner setup
		setupPolyDda(poly, dda, ivert, dir);

		// compute initial u,v tmap coords; 
		// scaling up to w,h of bitmap would be an optimization
		// but force textures to have bound sizes
		ATmapCoord& tc = poly.pTmapCoords[dda.vertIndex];
		dda.tc.u = tc.u;		// * tmap.width;
		dda.tc.v = tc.v;		// * tmap.height;

		// compute texture map delta
		ATmapCoord& tcn = poly.pTmapCoords[dda.vertNext];
		ATmapCoord tcNext(tcn.u, tcn.v);
		int ydelta = dda.yend - dda.ybeg;
		if (ydelta > 0) {
			dda.tcDelta.u = (tcNext.u - dda.tc.u) / ydelta;
			dda.tcDelta.v = (tcNext.v - dda.tc.v) / ydelta;
		}
	}

	ACanvas(int w, int h)
		:GCanvas(w, h),
		clipRect(0,0,w-1,h-1)
	{
	}

	void setTexture(std::shared_ptr<Texture> tx)
	{
		fTexture = tx;
	}

	// Some drawing commands
	virtual void background(const Pixel& c)
	{
		save();
		setFillStyle(c);
		fillAll();
		restore();
	}

	void clear()
	{
		clearRect(0, 0, (double)getWidth(), (double)getHeight());
		sync();
	}

	void drawBLine(int y, int beginX, int endX)
	{
		auto p = fTexture->pixelValue(0, 0, { 0,0,0 });

		setStrokeStyle(p);
		strokeLine(BLLine(beginX, y, endX, y));
	}

	void drawHLine(int y, int beginX, int endX)
	{
		vec3 c;
		Pixel p;

		for (int x = beginX; x < endX; x++)
		{
			// calculate u, v from texture
			float u = float(x) / (getWidth()-1);
			float v = float(y) / (getHeight() - 1);
			
			// get color value from texture
			p = fTexture->pixelValue(u, v, { 0,0,0 });

			// draw single pixel
			set(x, y, p);
		}
	}

	void drawTmapRow(int y, int xleft, int xright, 
		std::shared_ptr<Texture> tmap, ATmapCoord &tc, ATmapCoord &tcStep)
	{
		for (int x = xleft; x < xright; x++) {
			auto p = tmap->pixelValue(tc.u, tc.v, { 0,0,0 });
			set(x, y, p);

			tc.u += tcStep.u;
			tc.v += tcStep.v;
		}
	}

	void drawFlatConvexPolygon(const APolygon& poly)
	{
		// find topmost vertex of the polygon, set index in vmin
		size_t vmin = poly.findTopmostPolyVertex();

		// set starting line
		APolyDda ldda, rdda;
		int y = int(poly.pVerts[poly.vertIndex[vmin]].y);
		ldda.yend = rdda.yend = y;

		// set up a polygon scanner for left side
		// starting from top vertex
		setupPolyDda(poly, ldda, vmin, +1);

		setupPolyDda(poly, rdda, vmin, -1);

		// set color for all subsequent drawing
		setTexture(poly.tmap);
		auto p = poly.tmap->pixelValue(0, 0, { 0,0,0 });
		push();
		setStrokeStyle(p);

		// scan until on line-drawer finishes
		while (true)
		{
			// check left side dda hitting end of run
			if (y >= ldda.yend)
			{
				if (y >= rdda.yend) {
					if (ldda.vertNext == rdda.vertNext)
						break;

					// check for flat bottom
					int vnext = rdda.vertNext - 1;

					if (vnext < 0)
						vnext = poly.numVerts - 1;

					if (vnext == ldda.vertNext)
						break;
				}

				setupPolyDda(poly, ldda, ldda.vertNext, +1);
			}

			// Check for right dda
			if (y >= rdda.yend)
				setupPolyDda(poly, rdda, rdda.vertNext, -1);

			// Fill span between two line-drawers
			// advance drawers when hit vertices
			if (y >= clipRect.y0) {
				strokeLine(BLLine((int)round(ldda.x), y, (int)round(rdda.x), y));
			}

			// Advance left and right x-coords by franctional amounts
			ldda.x += ldda.dx;
			rdda.x += rdda.dx;

			// Advance ypos, exit if run off its bottom
			if (++y >= clipRect.y1)
				break;
		}
		pop();
	}

	void drawTmappedConvexPolygon(const APolygon& poly)
	{
		// find topmost vertex of the polygon, set index in vmin
		size_t vmin = poly.findTopmostPolyVertex();

		// set starting line
		APolyDda ldda, rdda;
		int y = int(poly.pVerts[poly.vertIndex[vmin]].y);
		ldda.yend = rdda.yend = y;

		// set up a polygon scanner for left side
		// starting from top vertex
		setupTmapPolyDda(poly, ldda, vmin, +1);

		// setup scanner for right side
		setupTmapPolyDda(poly, rdda, vmin, -1);

		// scan until on line-drawer finishes

		while (true)
		{
			// check left side dda hitting end of run
			if (y >= ldda.yend)
			{
				if (y >= rdda.yend) {
					if (ldda.vertNext == rdda.vertNext)
						break;

					// check for flat bottom
					int vnext = rdda.vertNext - 1;

					if (vnext < 0)
						vnext = poly.numVerts - 1;

					if (vnext == ldda.vertNext)
						break;
				}

				setupTmapPolyDda(poly, ldda, ldda.vertNext, +1);
			}

			// Check for right dda
			if (y >= rdda.yend)
				setupTmapPolyDda(poly, rdda, rdda.vertNext, -1);

			// Fill span between two line-drawers
			// advance drawers when hit vertices
			if (y >= clipRect.y0) {
				int xleft = std::round(ldda.x);
				int xright = std::round(rdda.x);
				int width = xright - xleft;
				if (width > 0) {
					// Setup and draw texture mapped row of polygon
					ATmapCoord tc = ldda.tc;
					ATmapCoord tcStep((rdda.tc.u - tc.u) / width, (rdda.tc.v - tc.v) / width);
					if (xleft < clipRect.x0) {
						tc.u += (tcStep.u);
						tc.v += (tcStep.v);
						xleft = clipRect.x0;
					}

					if (xright > clipRect.x1)
						xright = clipRect.x1;

					drawTmapRow(y, xleft, xright, poly.tmap, tc, tcStep);
				}

			}

			// Advance left and right x-coords by franctional amounts
			ldda.x += ldda.dx;
			rdda.x += rdda.dx;

			// advance left and right tmap coords
			ldda.tc.u += ldda.tcDelta.u;
			ldda.tc.v += ldda.tcDelta.v;
			rdda.tc.u += rdda.tcDelta.u;
			rdda.tc.v += rdda.tcDelta.v;

			// Advance ypos, exit if run off its bottom
			if (++y >= clipRect.y1)
				break;
		}

	}
};

struct AScene {
	static AScene* pFirst;

	enum class UpdateStrategy : unsigned
	{
		FULLAREA,
		SLOPPY,
		EXACT,
		UNCHANGED
	};

private:
	AScene* next;
	AScene* prev;
	Pixel fBackground;		// Background color
	AFix xScale;
	AFix yScale;
	AFix xWorld;
	AFix yWorld;

	AActor* fFrontActor;
	AActor* fRearActor;

	double tLastUpdate;		// time of last update

protected:
	ACanvas* pcv;
	ARect screenArea;
	ARectList updateAreas;
	UpdateStrategy upStrategy;
	bool scaling;

	void compose(ARect& area);				// recompose
	virtual void RedrawAndBlit() = 0;		// recompose and update screen
	virtual void Blit(ARect& area) = 0;		// update screen
};

struct AActor {
	AActor* prev;
	AActor* next;

	AScene* pscene;
};
