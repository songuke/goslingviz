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

public:
	void update(int delta);
	void sleep();
	void onKey(int key);
protected:
	void renderBuffer(Chunk& c, Rect r);

protected:
	Curve curve;
	Warp warp;

	ImageManager* imageManager;
	Image* imageOut;
	Image* scaledBackground;
	Image* background;

	int warpType;
	static int nextWarpTime;

	int curveType;
	static int nextCurveTime;
	long nextCurveTimeElapsed;

	bool drawCurve;
	static int drawCurveTime;
	long drawCurveTimeElapsed;
};
}
#endif