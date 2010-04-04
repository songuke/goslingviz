#ifndef _GOS_CURVE_
#define _GOS_CURVE_

#include "GosMustHave.h"

namespace Gos
{
/**
Curve is represented in parametric form:
	x = f(t)
	y = g(t)
where t is the time value.
*/
class Curve
{
public:
	Curve(void);
	~Curve(void);

public:
	virtual void update(int delta);
	virtual void render(Image* image, Chunk& c);

protected:
	int t;		// time elapsed
};
}

#endif
