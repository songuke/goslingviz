#ifndef _GOS_CURVE_FUNC_
#define _GOS_CURVE_FUNC_

#include "GosMustHave.h"

namespace Gos
{

class CurveFunc
{
public:
	CurveFunc(void);
	virtual ~CurveFunc(void);
	
public:
	static inline Float2 circle(float t, float r) {
		Float2 p;
		p.x = r * cos(t);
		p.y = r * sin(t);
		return p;
	}

	static inline Float2 heart(float t, float a) {
		Float2 p;
		//float r = 10.0f * a * ((sin(t) * sqrt(fabs(cos(t)))) / (sin(t) + 7/5) - 2 * sin(t) + 2);
		float r = 5.0f * a * (1 - cos(t));
		p.x = r * cos(t);
		p.y = r * sin(t);
		return p;
	}

	static inline Float2 flower(float t, float a) {
		Float2 p;
		int n = 4;
		float r = 10.0f * a * sin(n * t);
		p.x = r * cos(t);
		p.y = r * sin(t);
		return p;
	}
};
}

#endif