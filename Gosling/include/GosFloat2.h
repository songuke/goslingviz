#ifndef _FLOAT2_H_
#define _FLOAT2_H_

#include "GosUtil.h"

namespace Gos {
	class Float2
	{
	public:
		float x, y; 

	public:	
		//-------------------------------------------------------------------
		// constructor
		//-------------------------------------------------------------------
		
		inline Float2(float _x = 0, float _y = 0) 
			: x(_x), y(_y) {
		}

		inline Float2(const Float2& f) 
			: x(f.x), y(f.y) {

		}

		inline ~Float2(void) {
		}

		//-------------------------------------------------------------------
		// arithmetic element-wise operation
		//-------------------------------------------------------------------

		inline Float2 operator+(const Float2 &f) const {
			return Float2(x + f.x, y + f.y);
		}

		inline Float2 operator-(const Float2 &f) const {
			return Float2(x - f.x, y - f.y);
		}

		inline Float2 operator*(const Float2 &f) const {
			return Float2(x * f.x, y * f.y);
		}

		inline Float2 operator/(const Float2 &f) const {
			rzAssert(f.x != 0);
			rzAssert(f.y != 0);
			return Float2(x / f.x, y / f.y);
		}

		inline Float2 operator*(const float f) const {
			return Float2(x * f, y * f);
		}

		inline Float2 operator/(const float f) const {
			rzAssert(f != 0);
			float r = 1.0f / f;
			return Float2(x * r, y * r);
		}

		inline Float2 operator-() const {
			return Float2(-x, -y);
		}

		inline Float2& operator+=(const Float2 &f) {
			x += f.x;
			y += f.y;
			return *this;
		}

		inline Float2& operator-=(const Float2 &f) {
			x -= f.x;
			y -= f.y;
			return *this;
		}

		inline Float2& operator*=(const Float2 &f) {
			x *= f.x;
			y *= f.y;
			return *this;
		}

		inline Float2& operator/=(const Float2 &f) {
			rzAssert(f.x != 0);
			rzAssert(f.y != 0);
			x /= f.x;
			y /= f.y;
			return *this;
		}

		inline Float2& operator*=(const float f) {
			x *= f;
			y *= f;
			return *this;
		}

		inline Float2& operator/=(const float f) {
			rzAssert(f != 0);	
			float r = 1.0f / f; // use reciprocal to avoid division 4 times
			x *= r;
			y *= r;
			return *this;
		}
		
		//----------------------------------------------
		// Member access operation
		//----------------------------------------------
		inline float operator[](const int index) const {
			rzAssert(index >= 0 && index <= 1);
			return (&x)[index];
		}

		inline float& operator[](const int index) { // no const
			rzAssert(index >= 0 && index <= 1);
			return (&x)[index];
		}

		//-----------------------------------------------
		// Assignment
		//-----------------------------------------------
		inline Float2& operator=(const Float2& v) {
			x = v.x;
			y = v.y;
			return *this;
		}

		inline Float2& operator=(float f) {
			x = y = f;
			return *this;
		}

		//-----------------------------------------------
		// Boolean operator
		//-----------------------------------------------
		inline bool operator==(const Float2& f) const {
			return (x == f.x && y == f.y);
		}

		inline bool operator!=(const Float2& f) const {
			return (x != f.x || y != f.y);
		}

		//-----------------------------------------------
		// Vector operation
		//-----------------------------------------------
		inline float dot(const Float2& v) const {
			return x * v.x + y * v.y;
		}

		inline float lengthSquared() const {
			return x * x + y * y;
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
		 This function return a normalized vector of current vector
		 */
		inline Float2 normalize() {
			//*this /= length();
			//return *this;
			float l = length();
			if (l > 0)
				return *this / l;
			else 
				return *this;
		}
	};
}
#endif