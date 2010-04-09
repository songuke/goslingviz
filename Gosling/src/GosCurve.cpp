#include "GosCurve.h"
#include "GosLine.h"
#include "GosChunk.h"
#include "GosImage.h"
#include "GosCurveFunc.h"

namespace Gos
{
Curve::Curve(void)
: t(0), imageSmall(0), mask(0), makeImageSmall(false)
{
	nbCurveTypes = 4;
}

Curve::~Curve(void)
{
	safeDel(imageSmall);
	safeDel(mask);
}

void Curve::setGrid(int stepI, int stepJ) {
	if (this->stepI == stepI && this->stepJ == stepJ) return;

	this->stepI = stepI;
	this->stepJ = stepJ;

	makeImageSmall = true;
}

void Curve::setUseMask(bool _useMask) {
	useMask = _useMask;
}

void Curve::update(int delta) {
	//t += delta;
}

void Curve::render(Image* imageIn, Chunk& c, int curveType) {
	//
	// imageSmall construction if needed
	//
	if (useMask) {
		if (makeImageSmall) {
			safeDel(imageSmall);
			imageSmall = new Image(imageIn->getWidth() / stepJ, imageIn->getHeight() / stepI, imageIn->getChannels());

			safeDel(mask);
			mask = new Image(imageIn->getWidth() / stepJ, imageIn->getHeight() / stepI, 1);

			makeImageSmall = false;
		}	
		mask->clear();
	}
	Image* image;
	if (useMask)
		image = imageSmall;
	else
		image = imageIn;

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

		if (useMask)
			Line::draw(image, p0, p1, Float4(1.0f, 0.0f, 0.0f, 1.0f), mask);
		else
			Line::draw(image, p0, p1, Float4(1.0f, 0.0f, 0.0f, 1.0f));
		
		p0 = p1;
	}

	// distribute drawn points from imageSmall to image	
	if (useMask) {
		int sh = imageSmall->getHeight();
		int sw = imageSmall->getWidth();
		for (int i = 0, y = 0; i < sh; ++i, y += stepI) {
			for (int j = 0, x = 0; j < sw; ++j, x += stepJ) {
				Float2 p(j, i);
				if ((mask->getPixel(p)).x > 0)
					imageIn->setPixel(Float2(x, y), imageSmall->getPixel(p));
			}
		}
	}
}

int Curve::getCurveCount() const {
	return nbCurveTypes;
}

}
