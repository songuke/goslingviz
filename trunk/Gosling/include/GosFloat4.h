#ifndef _FLOAT4_H_
#define _FLOAT4_H_

#include "GosUtil.h"

namespace Gos {
	class Float4
	{
	public:
		float x, y, z, w; 

	public:	
		//-------------------------------------------------------------------
		// constructor
		//-------------------------------------------------------------------
		
		inline Float4(float _x = 0, float _y = 0, float _z = 0, float _w = 0) 
			: x(_x), y(_y), z(_z), w(_w) {
		}

		inline Float4(const Float4& f) 
			: x(f.x), y(f.y), z(f.z), w(f.w) {

		}

		inline ~Float4(void) {
		}

		//-------------------------------------------------------------------
		// arithmetic element-wise operation
		//-------------------------------------------------------------------

		inline Float4 operator+(const Float4 &f) const {
			return Float4(x + f.x, y + f.y, z + f.z, w + f.w);
		}

		inline Float4 operator-(const Float4 &f) const {
			return Float4(x - f.x, y - f.y, z - f.z, w - f.w);
		}

		inline Float4 operator*(const Float4 &f) const {
			return Float4(x * f.x, y * f.y, z * f.z, w * f.w);
		}

		inline Float4 operator/(const Float4 &f) const {
			rzAssert(f.x != 0);
			rzAssert(f.y != 0);
			rzAssert(f.z != 0);
			rzAssert(f.w != 0);
			return Float4(x / f.x, y / f.y, z / f.z, w / f.w);
		}

		inline Float4 operator*(const float f) const {
			return Float4(x * f, y * f, z * f, w * f);
		}

		inline Float4 operator/(const float f) const {
			rzAssert(f != 0);
			float r = 1.0f / f;
			return Float4(x * r, y * r, z * r, w * r);
		}

		inline Float4 operator-() const {
			return Float4(-x, -y, -z, -w);
		}

		inline Float4& operator+=(const Float4 &f) {
			x += f.x;
			y += f.y;
			z += f.z;
			w += f.w;
			return *this;
		}

		inline Float4& operator-=(const Float4 &f) {
			x -= f.x;
			y -= f.y;
			z -= f.z;
			w -= f.w;
			return *this;
		}

		inline Float4& operator*=(const Float4 &f) {
			x *= f.x;
			y *= f.y;
			z *= f.z;
			w *= f.w;
			return *this;
		}

		inline Float4& operator/=(const Float4 &f) {
			rzAssert(f.x != 0);
			rzAssert(f.y != 0);
			rzAssert(f.z != 0);
			rzAssert(f.w != 0);
			x /= f.x;
			y /= f.y;
			z /= f.z;
			w /= f.w;
			return *this;
		}

		inline Float4& operator*=(const float f) {
			x *= f;
			y *= f;
			z *= f;
			w *= f;
			return *this;
		}

		inline Float4& operator/=(const float f) {
			rzAssert(f != 0);	
			float r = 1.0f / f; // use reciprocal to avoid division 4 times
			x *= r;
			y *= r;
			z *= r;
			w *= r;
			return *this;
		}
		
		//----------------------------------------------
		// Member access operation
		//----------------------------------------------
		inline float operator[](const int index) const {
			rzAssert(index >= 0 && index <= 3);
			return (&x)[index];
		}

		inline float& operator[](const int index) { // no const
			rzAssert(index >= 0 && index <= 3);
			return (&x)[index];
		}

		//-----------------------------------------------
		// Assignment
		//-----------------------------------------------
		inline Float4& operator=(const Float4& v) {
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
			return *this;
		}

		inline Float4& operator=(float f) {
			x = y = z = w = f;
			return *this;
		}

		//-----------------------------------------------
		// Boolean operator
		//-----------------------------------------------
		inline bool operator==(const Float4& f) const {
			return (x == f.x && y == f.y && z == f.z && w == f.w);
		}

		inline bool operator!=(const Float4& f) const {
			return (x != f.x || y != f.y || z != f.z || w != f.w);
		}

		//-----------------------------------------------
		// Vector operation
		//-----------------------------------------------
		inline float dot(const Float4& v) const {
			return x * v.x + y * v.y + z * v.z + w * v.w;
		}

		inline float lengthSquared() const {
			return x * x + y * y + z * z + w * w;
		}

		inline float length() const {
			return sqrtf(lengthSquared());
		}

		//------------------------------------------------
		// Internal memory access
		//------------------------------------------------
		inline const float* getPtr() const {
			return &x;
		}

		/**
		@description
		 This function return normalized vector of current vector
		 */
		inline Float4 normalize() {
			return *this / length();
		}
	};
}
#endif