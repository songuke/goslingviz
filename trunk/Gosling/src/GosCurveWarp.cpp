#include "GosCurveWarp.h"
#include "GosImageManager.h"
#include "GosImage.h"

namespace Gos
{
int CurveWarp::nextWarpTime = 5000;
int CurveWarp::drawCurveTime = 500;
int CurveWarp::nextCurveTime = 5000;
CurveWarp::CurveWarp(void)
: imageOut(0), scaledBackground(0), warpType(1), drawCurveTimeElapsed(0), drawCurve(false), nextCurveTimeElapsed(0), curveType(0)
{
	imageManager = ImageManager::instance();
	background = imageManager->load("./images/cloud.png");
}

CurveWarp::~CurveWarp(void)
{
	safeDel(imageOut);
	safeDel(scaledBackground);
}

void CurveWarp::sleep() {
	sleepMiliseconds(1);
}

void CurveWarp::update(int delta) {
	warp.update(delta);

	while (timeElapsed > nextWarpTime) {
		warpType = (warpType + 1) % warp.getWarpCount();		
		timeElapsed -= nextWarpTime;

		// reset background
		//if (warpType == 1)
			//buffer->copy(scaledBackground);
		printf("Warp changed: %d\n", warpType);
	}

	drawCurveTimeElapsed += delta;
	while (drawCurveTimeElapsed > drawCurveTime) {
		drawCurveTimeElapsed -= drawCurveTime;
		drawCurve = true;
	}

	nextCurveTimeElapsed += delta;
	if (nextCurveTimeElapsed > nextCurveTime) {
		nextCurveTimeElapsed -= nextCurveTime;
		curveType = (curveType + 1) % curve.getCurveCount();
		printf("Curve changed: %d\n", curveType);
	}

	Visualizer2D::update(delta);
}

void CurveWarp::renderBuffer(Chunk& c, Rect r) {
	static bool firstCurveWarp = true;

	if (imageOut == 0) {
		imageOut = buffer->clone();
		scaledBackground = buffer->clone();
		scaledBackground->scaleFrom(background);
	} else {
		if (imageOut->isSameSize(buffer) == false)
		{		
			safeDel(imageOut);
			imageOut = buffer->clone();

			safeDel(scaledBackground);
			scaledBackground = buffer->clone();
			scaledBackground->scaleFrom(background);

			firstCurveWarp = true;
		}
	}

	// background
	if (firstCurveWarp) {
		//byte* pixels	= buffer->getPixels();
		//int width		= buffer->getWidth();
		//int height		= buffer->getHeight();
		//int bufferSize = width * height * 3;	
	
		
		// TODO: make buffer adapt to background size by adding a scale function and interpolation to image.
		// e.g., scaleTo(Size, Image* imageOut);
		//buffer->copy(background);		
		buffer->copy(scaledBackground);

		/*
		for (int i = 0; i < bufferSize; ++i) {
			pixels[i] = 255 * (i * 1.0f / bufferSize);
		}*/
	}

	// draw a curve
	if (drawCurve) {
		printf("Draw curve\n");
		curve.render(buffer, c, curveType);
		drawCurve = false;
	}

	// warp
	warp.render(buffer, imageOut, c, warpType);

	// copy back
	buffer->copy(imageOut);

	if (firstCurveWarp)
		firstCurveWarp = false;
}

void CurveWarp::onKey(int key) {
	if (key >= '0' && key <= '9') {
		warpType = key - '0';
		return;
	}
	switch (key) {
		case 13:
			buffer->copy(scaledBackground);
			break;
	}
}

}
