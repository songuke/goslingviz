#ifndef _GOS_CURVE_FUNC_
#define _GOS_CURVE_FUNC_

#include "GosMustHave.h"
#include "GosChunk.h"

namespace Gos
{

class CurveFunc
{
public:
	CurveFunc(void);
	virtual ~CurveFunc(void);
	
public:
	static inline Float2 circle(float t, float a, Chunk& c) {
		Float2 p;
		p.x = a * cos(t);
		p.y = a * sin(t);
		return p;
	}

	static inline Float2 heart(float t, float a, Chunk& c) {
		Float2 p;
		//float r = 10.0f * a * ((sin(t) * sqrt(fabs(cos(t)))) / (sin(t) + 7/5) - 2 * sin(t) + 2);
		float r = a * (1 - sin(t));
		p.x = r * cos(t);
		p.y = r * sin(t);
		return p;
	}

	static inline Float2 flower(float t, float a, Chunk& c) {
		Float2 p;
		int n = floor(a * 4) + 4;
		float r = a * sin(n * t);
		p.x = r * cos(t);
		p.y = r * sin(t);
		return p;
	}

	static inline Float2 oscilloscope(float t, float a, Chunk& c) {
		Float2 q;
		q.x = 2*t - 1;
		int index = t * (kChunkSize - 1);
		q.y = 0.4f * c.amplitude[index][0];
		return q;
	}
};
}

#endif