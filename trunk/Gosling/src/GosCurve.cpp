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
	nbCurveTypes = 4;
}

Curve::~Curve(void)
{
}

void Curve::update(int delta) {
	//t += delta;
}

void Curve::render(Image* image, Chunk& c, int curveType) {
	double maxAmp = 0.0;
	for (int i = 0; i < kChunkSize; ++i)
		maxAmp = std::max(maxAmp, fabs(c.amplitude[i][0]));
	float r = maxAmp;
	
	Float2 p0, p1;

	double t0, t1;
	switch (curveType) {
		// draw a circle			
		case 0:
		{		
			t0 = 0;
			t1 = 2 * M_PI;
			func = &CurveFunc::circle;
		}
		break;
		
		// draw a heart
		case 1:
		{
			t0 = 0;
			t1 = 2 * M_PI;
			func = &CurveFunc::heart;
		}
		break;

		// draw a flower
		case 2:
		{
			t0 = 0;
			t1 = 2 *  M_PI;
			func = &CurveFunc::flower;
		}

		case 3:
		{
			t0 = 0;
			t1 = 1;
			func = &CurveFunc::oscilloscope;
		}
		break;
	}
	double t = t0;
	int steps = 32;
	double step = (t1 - t0) / steps;

	p0 = func(t, r, c);
	p0 = image->transformLowerLeft(p0);
	while (t < t1) {
		t = std::min(t += step, t1);
		p1 = func(t, r, c);		

		// change to coordinate system from center to top left
		p1 = image->transformLowerLeft(p1);

		Line::draw(image, p0, p1, Float4(1.0f, 0.0f, 0.0f, 1.0f));
		p0 = p1;
	}
}

int Curve::getCurveCount() const {
	return nbCurveTypes;
}

}
