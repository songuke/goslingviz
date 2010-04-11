#include "GosCurveWarp.h"
#include "GosImageManager.h"
#include "GosImage.h"

namespace Gos
{
int CurveWarp::nextWarpTime = 5000;
int CurveWarp::drawCurveTime = 500;
int CurveWarp::nextCurveTime = 5000;
CurveWarp::CurveWarp(void)
: imageOut(0), imageTmp(0), scaledBackground(0), warpType(0), drawCurveTimeElapsed(0), drawCurve(false), nextCurveTimeElapsed(0), curveType(0)
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
		timeElapsed -= nextWarpTime;

		// reset background after some time
		// TODO: blend it to avoid abrupt change.
		// TODO: randomly choose another background.
		buffer->copy(scaledBackground);
		
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

	int stepI = 4;
	int stepJ = stepI;

	buffer->setGrid(stepI, stepJ);
	imageOut->setGrid(stepI, stepJ);

	// draw a curve	
	if (drawCurve) {
		printf("Draw curve\n");
		curve.setUseMask(true);
		curve.setGrid(stepI, stepJ);
		curve.render(buffer, c, curveType);		
	}

	// blur out the buffer before warp
	//this->blur(buffer, imageOut, Float2(stepJ, stepI));

	// warp	
	warp.setGrid(stepI, stepJ);
	warp.render(buffer, imageOut, c, warpType);

	// copy back
	buffer->copy(imageOut);
	
	// draw the curve again	
	/*
	if (drawCurve) {
		printf("Draw curve\n");
		curve.setUseMask(false);
		curve.render(buffer, c, curveType);		
	}*/
	
	if (drawCurve)
		drawCurve = false;

	if (firstCurveWarp)
		firstCurveWarp = false;
}

void CurveWarp::blur(Image* imageIn, Image* imageOut, Float2& kernel) {
	if (imageTmp) {
		if (imageTmp->isSameSize(imageIn) == false) {
			safeDel(imageTmp);
			imageTmp = imageIn->clone();
		}
	} else {
		imageTmp = imageIn->clone();
	}

	Float4 sum, color;
	int width = imageIn->getWidth();
	int height = imageIn->getHeight();
	int total = 0;
	int kh = kernel.y;
	int kw = kernel.x;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			sum = Float4();
			total = 0;
			for (int k = -kw; k <= kw; ++k) {
				int jj = j + k;
				if (jj < 0) jj = 0;
				if (jj >= width) jj = width - 1;
				color = imageIn->getPixel(Float2(jj, i));
				sum += color;
				++total;
			}
			sum /= total;
			imageTmp->setPixel(Float2(j, i), sum);
		}
	}

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			sum = Float4();
			total = 0;
			for (int k = -kh; k <= kh; ++k) {
				int ii = i + k;
				if (ii < 0) ii = 0;
				if (ii >= height) ii = height - 1;
				color = imageTmp->getPixel(Float2(j, ii));
				sum += color;
				++total;
			}
			sum /= total;
			imageOut->setPixel(Float2(j, i), sum);
		}
	}
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

void CurveWarp::onBeat() {
	printf("Beat detected!\n"); //placeholder

	// change warp type
	warpType = (warpType + 1) % warp.getWarpCount();
	printf("Warp changed: %d\n", warpType);

}

}
