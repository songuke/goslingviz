#include "GosWarp.h"
#include "GosImage.h"
#include "RenzoTimer.h"

using namespace Renzo;
Timer warpTimer;

namespace Gos
{
Warp::Warp(void)
: lookup(0), width(0), height(0), timeElapsed(0), stepI(1), stepJ(1), imageSmallIn(0), imageSmallOut(0)
{
	nbWarpTypes = 5;
}

Warp::~Warp(void)
{
	if (lookup) {
		for (int k = 0; k < nbWarpTypes; ++k) {
			for (int i = 0; i < height; ++i) {
				safeDelArray(lookup[k][i]);
			}
			safeDelArray(lookup[k]);
		}
		safeDelArray(lookup);
	}
}

void Warp::update(int delta) {
	this->delta = delta;

	timeElapsed += delta;
}
/*
void Warp::setSize(int _width, int _height) {
	if (this->width == _width && this->height == _height) return;
	width = _width;
	height = _height;

	// reconstruct lookup table
	if (lookup) {
		for (int k = 0; k < nbWarpTypes; ++k) {
			for (int i = 0; i < height; ++i) {
				safeDelArray(lookup[k][i]);
			}
			safeDelArray(lookup[k]);
		}
		safeDelArray(lookup);
	}

	lookup = new Float2**[nbWarpTypes];
	for (int k = 0; k < nbWarpTypes; ++k) {
		lookup[k] = new Float2*[height];
		for (int i = 0; i < height; ++i) {
			lookup[k][i] = new Float2[width];
		}
	}
	
	for (int k = 0; k < nbWarpTypes; ++k) {
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {				
				lookup[k][i][j] = warp(Float2(j, i), k);
			}
		}
	}

}*/

void Warp::render(Image* imageIn, Image* imageOut, int stepI, int stepJ, Chunk &c, int warpType) {
	
	//this->setSize(width, height); // construct lookup table if required

	// for every pixel in the output image, find the location in the input image
	// and perform bilinear sampling	
	//imageIn->setGrid(stepI, stepJ);
	//imageOut->setGrid(stepI, stepJ);

	//imageIn->interpolateAtGrid();
	//imageOut->copy(imageIn);
	//return;
	
	Image* in, *out;

	if (! (this->stepI == stepI && this->stepJ == stepJ)) {
		this->stepI = stepI;
		this->stepJ = stepJ;

		// make a new temp image
		safeDel(imageSmallIn);
		safeDel(imageSmallOut);
		imageSmallIn = new Image(imageIn->getWidth() / stepJ, imageIn->getHeight() / stepI, imageIn->getChannels());
		imageSmallOut = new Image(imageIn->getWidth() / stepJ, imageIn->getHeight() / stepI, imageIn->getChannels());
	}

	if ((stepI == 1 && stepJ == 1)) {
		in = imageIn;
		out = imageOut;
	} else {
		in = imageSmallIn;
		out = imageSmallOut;

		// now down sample the imageIn
		imageIn->downSample(imageSmallIn);
	}

	// do warp in imageSmall
	warpTimer.Reset();
	
	int width = out->getWidth();
	int height = out->getHeight();

	//Float2 s(stepJ, stepI);
	//Float2 s_rep(1.0f / stepJ, 1.0f / stepI);
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			Float2 u(j, i);
			Float2 p = out->transformCenter(u);
			Float2 q = warp(p, warpType);
			//Float2 q = warp(p * s_rep, warpType) * s;
			Float2 v = in->transformLowerLeft(q);
			Float4 color = in->getPixelBilinear(v);
			
			out->setPixel(u, color);
		}
	}
	warpTimer.Update();
	//printf("Warp time: %ld\n", warpTimer.GetTimeElapsed());
	
	if (! (stepI == 1 && stepJ == 1)) {
		// up sample from imageSmallOut
		imageSmallOut->upSample(imageOut);
	}

	/*
	warpTimer.Reset();
	if (stepI > 1)
		imageOut->interpolateAtGrid();
	warpTimer.Update();
	*/
	//printf("Up sampling time: %ld\n", warpTimer.GetTimeElapsed());
}

}