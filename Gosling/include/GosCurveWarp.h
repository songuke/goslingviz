#ifndef _GOS_CURVE_WARP_
#define _GOS_CURVE_WARP_

#include "GosMustHave.h"
#include "GosVisualizer2D.h"
#include "GosCurve.h"
#include "GosWarp.h"

namespace Gos
{
class CurveWarp : public Visualizer2D
{
public:
	CurveWarp(void);
	~CurveWarp(void);

protected:
	void renderBuffer(Chunk& c, Rect r);
	void sleep();

protected:
	Curve curve;
	Warp warp;

	Image* imageOut;
};
}
#endif