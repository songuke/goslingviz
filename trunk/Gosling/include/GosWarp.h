#ifndef _GOS_WARP_
#define _GOS_WARP_

#include "GosMustHave.h"

namespace Gos
{

class Warp
{
public:
	Warp(void);
	~Warp(void);

public:
	void update(int delta);
	void render(Image* imageIn, Image* imageOut, int stepI, int stepJ, Chunk &c, int warpType);
	void setSize(int width, int height);
	//void setGrid(int stepI, int stepJ);

protected:
	Float2*** lookup;	// lookup table
	int nbWarpTypes;	// the number of warp types
	int width, height;

public:
	inline int getWarpCount() const {
		return nbWarpTypes;
	}
protected:
	/*
	inline Float2 Warp::warp(Float2 p, int warpType) {
		return lookup[warpType][(int)p.y][(int)p.x];
	}
	*/

	//inline Float2 Warp::_warp(Float2 p, int warpType) {
	inline Float2 Warp::warp(Float2 p, int warpType) {
		Float2 q;
		float fDelta = delta * 0.01f;
		switch(warpType) {
			case 0: 
			{	
				float v = 0.05 * fDelta;
				Float2 n = p.normalize();
				q = p - n * v;
				
				//q = p;
			}
			break;

			case 1:
			{
				// 
				// spiral (center more)
				//			
				float a = 0.1f * fDelta, b = 1;
				float angle = a * exp(-(p.x*p.x + p.y*p.y) / (b*b));
				q.x = cos(angle)*p.x + sin(angle)*p.y;
				q.y = -sin(angle)*p.x + cos(angle)*p.y;		
			}
			break;

			case 2:
			{
				// 
				// spiral 2 (edge more)
				//			
				float a = 0.1f * fDelta, b = 1;
				float angle = a * (p.x*p.x + p.y*p.y) / (b*b);
				q.x = cos(angle)*p.x + sin(angle)*p.y;
				q.y = -sin(angle)*p.x + cos(angle)*p.y;		
			}
			break;

			case 3:
			{
				float r = sqrt(p.x*p.x + p.y*p.y);
				float r_1 = 1.0f / r;
				float cosa = p.x * r_1;
				float sina = p.y * r_1;
				//float r2 = sqrt(r*r*r);
				//float r2 = sqrt(r*3);
				//float r2 = sqrt(pow(r, 1.9f));
				float r2 = sin(M_PI * r * 0.25f);
				q.x = r2 * cosa;
				q.y = r2 * sina;				
			}
			break;

			case 4:
			{
				float r = sqrt(p.x*p.x + p.y*p.y);
				float r_1 = 1.0f / r;
				float cosa = p.x * r_1;
				float sina = p.y * r_1;
				float r2 = r*sqrt(r);
				q.x = r2 * cosa;
				q.y = r2 * sina;				
			}
			break;

			case 5:
			{
				/*
				float r = sqrt(p.x*p.x + p.y*p.y);
				float n = r;
				float c = 1.0f;
				float r2 = c * sqrt(n);
				float alpha = n * 137.5f * M_PI / 180.0f;
				*/
				float r = sqrt(p.x*p.x + p.y*p.y);
				float r_1 = 1.0f / r;
				float cosa = p.x * r_1;				
				float sina = p.y * r_1;
				float alpha = 0.1f + atan(p.y / p.x);
				float r2 = 1.0f / sqrt(alpha);
				q.x = r2 * cosa;
				q.y = r2 * sina;	
				
				/*
				float t = timeElapsed * 0.001f;
				float r = sqrt(p.x*p.x + p.y*p.y);

				q.x = p.x + r * sin(M_PI * t);
				q.y = p.y + r * sin(M_PI * t);
				*/				
			}
			break;
		}
		return q;		
	}

protected:
	//long timeElapsed;	
	//int warpType;
	int delta;
	long timeElapsed;
	int stepI, stepJ;

	Image* imageSmallIn;
	Image* imageSmallOut;
};
}

#endif