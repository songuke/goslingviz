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
	int getCurveCount() const;
	void setGrid(int stepI, int stepJ);
	void setUseMask(bool _useMask);
	void setCurveType(int curveType);

protected:
	int t;		// time elapsed
	int nbCurveTypes;
	int curveType;
	Float2 (*func)(float t, float r, Chunk& c);
	double t0, t1;

	int stepI, stepJ;
	Image* imageSmall;
	Image* mask;
	bool makeImageSmall;
	bool useMask;


};
}

#endif
