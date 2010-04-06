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
	void render(Image* imageIn, Image* imageOut, Chunk &c, int warpType);
protected:
	inline Float2 Warp::warp(Float2 p, int warpType) {
		Float2 q;
		float fDelta = delta * 0.01f;
		switch(warpType) {
			case 0: 
			{	
				//float v = 0.05f * fDelta;				
				float v = 0.0f;
				Float2 n = p.normalize();
				q = p - n * v;
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
				float cosa = p.x / r;
				float sina = p.y / r;
				//float r2 = sqrt(r*r*r);
				//float r2 = sqrt(r*3);
				//float r2 = sqrt(pow(r, 1.9f));
				float r2 = 0.1f * sin(M_PI * r * 0.25f) * fDelta;
				q.x = r2 * cosa;
				q.y = r2 * sina;				
			}
			break;

			case 4:
			{
				float r = sqrt(p.x*p.x + p.y*p.y);
				float cosa = p.x / r;
				float sina = p.y / r;
				float r2 = sqrt(r*r*r);
				q.x = r2 * cosa;
				q.y = r2 * sina;				
			}
			break;
		}
		return q;		
	}

protected:
	//long timeElapsed;	
	//int warpType;
	int delta;
};
}

#endif