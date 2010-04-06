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
	void update(int delta);
	void renderBuffer(Chunk& c, Rect r);
	void sleep();
	void onKey(int key);

protected:
	Curve curve;
	Warp warp;

	ImageManager* imageManager;
	Image* imageOut;
	Image* scaledBackground;
	Image* background;

	int warpType;
	static int nextWarpTime;
};
}
#endif