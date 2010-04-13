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
	void onBeat();
	void onFileChanged(const String& file);

	//void render(Chunk& c, Rect r); // override to implement ping-pong buffer
protected:
	
	void renderBuffer(Chunk& c, Rect r);
	
	void blur(Image* imageIn, Image* imageOut, Float2& k);
protected:
	Curve curve;
	Warp warp;

	ImageManager* imageManager;
	Image* imageOut;
	Image* scaledBackground;
	Image* background;
	Image* imageTmp;

	int warpType;
	static int nextWarpTime;

	int curveType;
	static int nextCurveTime;
	long nextCurveTimeElapsed;
	static double thresDrawCurve;

	bool drawCurve;
	static int drawCurveTime;
	long drawCurveTimeElapsed;
};
}
#endif