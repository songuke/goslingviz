#include "GosCurve.h"
#include "GosLine.h"
#include "GosChunk.h"
#include "GosImage.h"
#include "GosCurveFunc.h"

namespace Gos
{
Curve::Curve(void)
: t(0)
{

}

Curve::~Curve(void)
{
}

void Curve::update(int delta) {
	t += delta;
}

void Curve::render(Image* image, Chunk& c) {
	double maxAmp = 0.0;
	for (int i = 0; i < kChunkSize; ++i)
		maxAmp = std::max(maxAmp, fabs(c.amplitude[i][0]));
	float r = (maxAmp + 1.0f) * image->getWidth() / 64;
	
	Float2 p0, p1;

	// draw a circle
	
	double t0 = 0;
	double t1 = 2 * M_PI;
	p0 = CurveFunc::circle(t0, r);
	
	// draw a heart
	/*
	double t0 = 0;
	double t1 = 2 * M_PI;
	p0 = CurveFunc::heart(t0, r);
	*/

	// draw a flower
	/*
	double t0 = 0;
	double t1 = 2 *  M_PI;
	p0 = CurveFunc::flower(t0, r);
	*/

	p0 = image->transformLowerLeft(p0);

	int steps = 120;
	double step = (t1 - t0) / steps;
	double t = t0;
	while (t < t1) {
		t = std::min(t += step, t1);

		p1 = CurveFunc::circle(t, r);
		//p1 = CurveFunc::heart(t, r);
		//p1 = CurveFunc::flower(t, r);
		
		// change to coordinate system from center to top left
		p1 = image->transformLowerLeft(p1);
	
		Line::draw(image, p0, p1, Float4(1.0f, 0.0f, 0.0f, 1.0f));
		p0 = p1;
	}
}

}
