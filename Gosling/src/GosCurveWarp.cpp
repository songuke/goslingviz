#include "GosCurveWarp.h"

namespace Gos
{
CurveWarp::CurveWarp(void)
: imageOut(0)
{
	
}

CurveWarp::~CurveWarp(void)
{
	safeDel(imageOut);
}

void CurveWarp::sleep() {
	sleepMiliseconds(1);
}

void CurveWarp::renderBuffer(Chunk& c, Rect r) {
	static bool firstCurveWarp = true;

	if (imageOut == 0)
		imageOut = buffer->clone();
	else
		if (imageOut->isSameSize(buffer) == false)
		{		
			safeDel(imageOut);
			imageOut = buffer->clone();

			firstCurveWarp = true;
		}

	// background
	if (firstCurveWarp) {
		byte* pixels	= buffer->getPixels();
		int width		= buffer->getWidth();
		int height		= buffer->getHeight();
		int bufferSize = width * height * 3;	

		for (int i = 0; i < bufferSize; ++i) {
			pixels[i] = 255 * (i * 1.0f / bufferSize);
		}
	}

	

	// draw a curve
	if (firstCurveWarp)
		curve.render(buffer, c);

	// warp
	warp.render(buffer, imageOut, c);

	// copy back
	buffer->copy(imageOut);

	if (firstCurveWarp)
		firstCurveWarp = false;
}

}
