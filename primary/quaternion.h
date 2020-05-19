#pragma once
/*
	Original Framework for this class comes from 
	Will Perone (will.perone@gmail.com) 2003 - 2012

	Several other references have been consulted to come up 
	with an optimal solution for the view3d program.

	References:
	Quaternions and Rotation Sequences
		By: Jack B. Kuipers

	https://en.wikipedia.org/wiki/Quaternion
	http://www.technologicalutopia.com/sourcecode/xnageometry/quaternion.cs.htm

*/

/****************************************
* Quaternion class
* By Will Perone (will.perone@gmail.com)
* Original: 12-09-2003
* Revised:  27-09-2003
*           22-11-2003
*           10-12-2003
*           15-01-2004
*           16-04-2004
*           07-29-2011    added corrections from website
*           22-12-2011    added correction to *= operator, thanks Steve Rogers
*           22-10-2012    fixed ctor from euler angles & added non windows platform fixes, thanks to Art Golf
*
* Dependancies: My 4x4 matrix class
*
* ? 2003, This code is provided "as is" and you can use it freely as long as
* credit is given to Will Perone in the application it is used in
*
* Notes:
* if |q|=1 then q is a unit quaternion
* if q=(0,v) then q is a pure quaternion
* if |q|=1 then q conjugate = q inverse
* if |q|=1 then q= [cos(angle), u*sin(angle)] where u is a unit vector
* q and -q represent the same rotation
* q*q.conjugate = (q.length_squared, 0)
* ln(cos(theta),sin(theta)*v)= ln(e^(theta*v))= (0, theta*v)
****************************************/


#include "geometry.h"
#include "assert.h"


struct quaternion
{
	float	s; // the real component
	Vec3f	v;	// the imaginary components


	// constructors
	quaternion() :s(0) {}
	quaternion(float x, float y, float z, float real) : s(real), v(x, y, z) {}
	quaternion(const Vec3f &i, float real) : s(real), v(i) {}

	// Construct from 3 euler angles
	// float heading, float attitude, float bank)
	quaternion(float theta_z, float theta_y, float theta_x) 
	{
		float cos_z_2 = cosf(0.5f*theta_z);
		float cos_y_2 = cosf(0.5f*theta_y);
		float cos_x_2 = cosf(0.5f*theta_x);

		float sin_z_2 = sinf(0.5f*theta_z);
		float sin_y_2 = sinf(0.5f*theta_y);
		float sin_x_2 = sinf(0.5f*theta_x);

		// and now compute quaternion
		s = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
		v.x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
		v.y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
		v.z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;
	}

	// Construct from 3 euler angles 
	quaternion(const Vec3f &angles)
		:quaternion(angles.z, angles.y, angles.x)
	{}


	// Assignment operator
	quaternion &operator =(const quaternion &q)
	{
		s = q.s; 
		v = q.v;	
		
		return *this;
	}

	// Arithmetic operations
	const quaternion operator +(const quaternion &q) const
	{
		return quaternion(v + q.v, s + q.s );
	}

	const quaternion operator -(const quaternion &q) const
	{
		return quaternion(v - q.v, s - q.s );
	}

	const quaternion operator *(const quaternion &q) const
	{
		return quaternion(
			v.y*q.v.z - v.z*q.v.y + s*q.v.x + v.x*q.s,
			v.z*q.v.x - v.x*q.v.z + s*q.v.y + v.y*q.s,
			v.x*q.v.y - v.y*q.v.x + s*q.v.z + v.z*q.s,
			s*q.s - v*q.v);
	}

	const quaternion operator /(const quaternion &q) const
	{
		quaternion p(q);
		p.invert();
		return *this * p;
	}

	const quaternion operator *(float scale) const
	{
		return quaternion(v*scale, s*scale);
	}

	const quaternion operator /(float scale) const
	{
		return quaternion(v / scale, s / scale );
	}

	const quaternion operator -() const
	{
		return quaternion(-v , -s );
	}

	const quaternion &operator +=(const quaternion &q)
	{
		v += q.v; 
		s += q.s; 
		
		return *this;
	}

	const quaternion &operator -=(const quaternion &q)
	{
		v -= q.v; 
		s -= q.s;

		return *this;
	}

	const quaternion &operator *=(const quaternion &q)
	{
		float x = v.x, y = v.y, z = v.z, sn = s*q.s - v*q.v;
		v.x = y*q.v.z - z*q.v.y + s*q.v.x + x*q.s;
		v.y = z*q.v.x - x*q.v.z + s*q.v.y + y*q.s;
		v.z = x*q.v.y - y*q.v.x + s*q.v.z + z*q.s;
		s = sn;

		return *this;
	}

	const quaternion &operator *= (float scale)
	{
		v *= scale; 
		s *= scale; 
		
		return *this;
	}

	const quaternion &operator /= (float scale)
	{
		v /= scale; 
		s /= scale; 
		
		return *this;
	}


	// gets the length of this quaternion
	float length() const
	{
		return (float)sqrt(s*s + v*v);
	}

	//! gets the squared length of this quaternion
	float length_squared() const
	{
		return (float)(s*s + v*v);
	}

	// Normalizes this quaternion
	void normalize()
	{
		*this /= length();
	}

	// Return a normalized version of this quaternion
	quaternion normalized() const
	{
		return  *this / length();
	}

	// Compute the conjugate of this quaternion
	void conjugate()
	{
		v = -v;
	}

	// Invert this quaternion
	void invert()
	{
		conjugate(); 
		*this /= length_squared();
	}

	//! returns the logarithm of a quaternion = v*a where q = [cos(a),v*sin(a)]
	quaternion log() const
	{
		float a = (float)acos(s);
		float sina = (float)sin(a);
		quaternion ret;

		ret.s = 0;
		if (sina > 0)
		{
			ret.v.x = a*v.x / sina;
			ret.v.y = a*v.y / sina;
			ret.v.z = a*v.z / sina;
		}
		else {
			ret.v.x = ret.v.y = ret.v.z = 0;
		}
		return ret;
	}

	//! returns e^quaternion = exp(v*a) = [cos(a),vsin(a)]
	quaternion exp() const
	{
		float a = v.norm();
		float sina = sin(a);
		float cosa = cos(a);
		quaternion ret;

		ret.s = cosa;
		if (a > 0)
		{
			ret.v.x = sina * v.x / a;
			ret.v.y = sina * v.y / a;
			ret.v.z = sina * v.z / a;
		}
		else {
			ret.v.x = ret.v.y = ret.v.z = 0;
		}
		return ret;
	}

	// casting to a 4x4 isomorphic matrix for right multiplication with vector
	operator Matrix() const
	{
		Matrix res;
		//return Matrix(s, -v.x, -v.y, -v.z,
		//	v.x, s, -v.z, v.y,
		//	v.y, v.z, s, -v.x,
		//	v.z, -v.y, v.x, s);
		
		return res;
	}

	// casting to a 4x4 isomorphic matrix for left multiplication with vector
	/*operator Matrix() const
	{
	return matrix4(   s, v.x, v.y, v.z,
	-v.x,   s,-v.z, v.y,
	-v.y, v.z,   s,-v.x,
	-v.z,-v.y, v.x,   s);
	}*/

	//! casting to 3x3 rotation matrix
	operator Matrix3() const
	{
		//assert(((length() > 0.9999) && (length() < 1.0001)), "quaternion is not normalized");
		Matrix3 res;
		float x2 = v.x * v.x;
		float y2 = (float)v.y * v.y;
		float z2 = (float)v.z * v.z;
		float xy = (float)v.x * v.y;
		float xz = (float)v.x * v.z;
		float yz = (float)v.y * v.z;
		float wx = (float)s * v.x;
		float wy = (float)s * v.y;
		float wz = (float)s * v.z;

		res[0] = Vec3f(1 - 2 * (y2 + z2), 2 * (xy - s*v.z), 2 * (xz + s*v.y));
		res[1] = Vec3f(2 * (xy + s*v.z), 1 - 2 * (v.x*v.x + v.z*v.z), 2 * (v.y*v.z - s*v.x));
		res[2] = Vec3f(2 * (v.x*v.z - s*v.y), 2 * (v.y*v.z + s*v.x), 1 - 2 * (v.x*v.x + v.y*v.y));

		return res;
/*
		return matrix3(1 - 2 * (v.y*v.y + v.z*v.z), 2 * (v.x*v.y - s*v.z), 2 * (v.x*v.z + s*v.y),
			2 * (v.x*v.y + s*v.z), 1 - 2 * (v.x*v.x + v.z*v.z), 2 * (v.y*v.z - s*v.x),
			2 * (v.x*v.z - s*v.y), 2 * (v.y*v.z + s*v.x), 1 - 2 * (v.x*v.x + v.y*v.y));
*/
	}

	// computes the dot product of 2 quaternions
	static inline float dot(const quaternion &q1, const quaternion &q2)
	{
		return q1.v*q2.v + q1.s*q2.s;
	}

	//! linear quaternion interpolation
	static quaternion lerp(const quaternion &q1, const quaternion &q2, float t)
	{
		return (q1*(1 - t) + q2*t).normalized();
	}

	//! spherical linear interpolation
	static quaternion slerp(const quaternion &q1, const quaternion &q2, float t)
	{
		quaternion q3;
		float dot = quaternion::dot(q1, q2);

		/*	dot = cos(theta)
		if (dot < 0), q1 and q2 are more than 90 degrees apart,
		so we can invert one to reduce spinning	*/
		if (dot < 0)
		{
			dot = -dot;
			q3 = -q2;
		}
		else q3 = q2;

		if (dot < 0.95f)
		{
			float angle = acosf(dot);
			return (q1*sinf(angle*(1 - t)) + q3*sinf(angle*t)) / sinf(angle);
		}
		else // if the angle is small, use linear interpolation								
			return lerp(q1, q3, t);
	}

	//! This version of slerp, used by squad, does not check for theta > 90.
	static quaternion slerpNoInvert(const quaternion &q1, const quaternion &q2, float t)
	{
		float dot = quaternion::dot(q1, q2);

		if (dot > -0.95f && dot < 0.95f)
		{
			float angle = acosf(dot);
			return (q1*sinf(angle*(1 - t)) + q2*sinf(angle*t)) / sinf(angle);
		}
		else  // if the angle is small, use linear interpolation								
			return lerp(q1, q2, t);
	}

	//! spherical cubic interpolation
	static quaternion squad(const quaternion &q1, const quaternion &q2, const quaternion &a, const quaternion &b, float t)
	{
		quaternion c = slerpNoInvert(q1, q2, t),
			d = slerpNoInvert(a, b, t);
		return slerpNoInvert(c, d, 2 * t*(1 - t));
	}

	//! calculate inner quad point for spherical cubic interpolation 2
	static quaternion innerQuadPoint(const quaternion &q0, const quaternion &q1, const quaternion &q2)
	{
		quaternion vTemp, vTemp1, vTemp2;
		quaternion qResult;

		vTemp = q1;
		vTemp.invert();

		vTemp1 = (vTemp * q0);
		vTemp1 = vTemp1.log();

		vTemp2 = (vTemp * q2);
		vTemp2 = vTemp2.log();

		vTemp = (vTemp1 + vTemp2) * (-0.25f);

		qResult = q1 * vTemp.exp();

		return qResult;
	}

	// spherical cubic interpolation alternate approach
	// this method is smoother than the original squad but only works when the angles are less than 90 degrees apart
	static quaternion squad2(const quaternion &q1, const quaternion &q2, const quaternion &q3, const quaternion &q4, float t)
	{
		quaternion a = innerQuadPoint(q1, q2, q3),
			b = innerQuadPoint(q2, q3, q4);
		return slerpNoInvert(slerpNoInvert(q2, q3, t), slerpNoInvert(a, b, t), 2.0f*t*(1.0f - t));
	}

	// Shoemake-Bezier interpolation using De Castlejau algorithm
	static quaternion bezier(const quaternion &q1, const quaternion &q2, const quaternion &a, const quaternion &b, float t)
	{
		// level 1
		quaternion q11 = slerpNoInvert(q1, a, t),
			q12 = slerpNoInvert(a, b, t),
			q13 = slerpNoInvert(b, q2, t);
		// level 2 and 3
		return slerpNoInvert(slerpNoInvert(q11, q12, t), slerpNoInvert(q12, q13, t), t);
	}

	//! Given 3 quaternions, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
	static quaternion spline(const quaternion &qnm1, const quaternion &qn, const quaternion &qnp1)
	{
		quaternion qni(-qn.v, qn.s);

		return qn * (((qni*qnm1).log() + (qni*qnp1).log()) / -4).exp();
	}

	//! converts from a normalized axis - angle pair rotation to a quaternion
	static inline quaternion from_axis_angle(const Vec3f &axis, float angle)
	{
		return quaternion(axis*sinf(angle / 2), cosf(angle / 2));
	}

	//! returns the axis and angle of this unit quaternion
	void to_axis_angle(Vec3f &axis, float &angle) const
	{
		angle = acosf(s);

		// pre-compute to save time
		float sinf_theta_inv = 1.0f / sinf(angle);

		// now the vector
		axis.x = v.x*sinf_theta_inv;
		axis.y = v.y*sinf_theta_inv;
		axis.z = v.z*sinf_theta_inv;

		// multiply by 2
		angle *= 2;
	}

	//! rotates v by this quaternion (quaternion must be unit)
	Vec3f rotate(const Vec3f &v)
	{
		quaternion V(v, 0);
		quaternion conjugate(*this);
		conjugate.conjugate();
		return (*this * V * conjugate).v;
	}

	//! returns the euler angles from a rotation quaternion
	Vec3f euler_angles(bool homogenous = true) const
	{
		float sqw = s*s;
		float sqx = v.x*v.x;
		float sqy = v.y*v.y;
		float sqz = v.z*v.z;

		Vec3f euler;
		if (homogenous) {
			euler.x = atan2f(2.f * (v.x*v.y + v.z*s), sqx - sqy - sqz + sqw);
			euler.y = asinf(-2.f * (v.x*v.z - v.y*s));
			euler.z = atan2f(2.f * (v.y*v.z + v.x*s), -sqx - sqy + sqz + sqw);
		}
		else {
			euler.x = atan2f(2.f * (v.z*v.y + v.x*s), 1 - 2 * (sqx + sqy));
			euler.y = asinf(-2.f * (v.x*v.z - v.y*s));
			euler.z = atan2f(2.f * (v.x*v.y + v.z*s), 1 - 2 * (sqy + sqz));
		}
		return euler;
	}
};

static const struct quaternion identity = quaternion(0, 0, 0, 1);


