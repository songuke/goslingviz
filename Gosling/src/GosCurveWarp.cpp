#include "GosMustHave.h"
#include "GosCurveWarp.h"
#include "GosImageManager.h"
#include "GosImage.h"
#include "GosChunk.h"

namespace Gos
{
int CurveWarp::nextWarpTime = 5000;
int CurveWarp::drawCurveTime = 500;
int CurveWarp::nextCurveTime = 5000;
double CurveWarp::thresDrawCurve = 0.1;

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

	/*
	while (timeElapsed > nextWarpTime) {
		timeElapsed -= nextWarpTime;

		// reset background after some time
		// TODO: blend it to avoid abrupt change.
		// TODO: randomly choose another background.
		//buffer->copy(scaledBackground);
		
	}*/

	/*
	drawCurveTimeElapsed += delta;
	while (drawCurveTimeElapsed > drawCurveTime) {
		drawCurveTimeElapsed -= drawCurveTime;
		drawCurve = true;
	}*/
	//
	
	/*
	nextCurveTimeElapsed += delta;
	if (nextCurveTimeElapsed > nextCurveTime) {
		nextCurveTimeElapsed -= nextCurveTime;
		curveType = (curveType + 1) % curve.getCurveCount();
		//printf("Curve changed: %d\n", curveType);
	}*/

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

	// ping-pong buffer
	Image* t = buffer;
	buffer = imageOut;
	imageOut = t;

	// background
	if (firstCurveWarp) {		
		buffer->copy(scaledBackground);
	}
	
	// draw a curve when there exists an amplitude that is larger than a threshold
	for (int i = 0; i < kChunkSize; ++i) {	
		//if (fabs(c.amplitude[i][0]) > thresDrawCurve || fabs(c.amplitude[i][1]) > thresDrawCurve) {			
		if (fabs(c.amplitude[i][0]) > thresDrawCurve) {			
			drawCurve = true;
			break;
		}
	}
	
	const int stepI = 4;
	const int stepJ = stepI;

	//buffer->setGrid(stepI, stepJ);
	//imageOut->setGrid(stepI, stepJ);

	// draw a curve	
	/*
	if (drawCurve) {
		printf("Draw curve\n");
		curve.setUseMask(false);
		//curve.setGrid(stepI, stepJ);
		curve.render(buffer, c, curveType);		
	}*/

	// blur out the buffer before warp
	//this->blur(buffer, imageOut, Float2(stepJ, stepI));

	// warp	
	//warp.setGrid(stepI, stepJ);
	//warp.render(buffer, imageOut, stepI, stepJ, c, warpType);
	warp.render(imageOut, buffer, stepI, stepJ, c, warpType);

	// copy back
	//buffer->copy(imageOut);
	
	// draw the curve again	
	if (drawCurve) {
		//printf("Draw curve\n");
		//curve.setUseMask(false);
		curve.render(buffer, c);		
	}
	
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
	warpType = randomInteger(0, warp.getWarpCount() - 1);
	
	// also random a curve type
	curveType = randomInteger(0, curve.getCurveCount() - 1);
	curve.setCurveType(curveType);
	
	// can blend it a little background if needed.
	//buffer->blend(scaledBackground, 0.1f);
}

void CurveWarp::onFileChanged(const String& file) {
	if (buffer)
		buffer->clear();
}

}
