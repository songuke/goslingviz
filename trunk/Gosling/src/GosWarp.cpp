#include "GosWarp.h"
#include "GosImage.h"
#include "RenzoTimer.h"

using namespace Renzo;
Timer warpTimer;

namespace Gos
{
Warp::Warp(void)
: lookup(0), width(0), height(0), timeElapsed(0)
{
	nbWarpTypes = 6;
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

}

void Warp::render(Image* imageIn, Image* imageOut, Chunk &c, int warpType) {
	int width = imageOut->getWidth();
	int height = imageOut->getHeight();
	//this->setSize(width, height); // construct lookup table if required

	// for every pixel in the output image, find the location in the input image
	// and perform bilinear sampling
	int stepI = 1;
	int stepJ = stepI;
	imageIn->setGrid(stepI, stepJ);
	imageOut->setGrid(stepI, stepJ);

	warpTimer.Reset();

	Float2 s(stepJ, stepI);
	Float2 s_rep(1.0f / stepJ, 1.0f / stepI);
	for (int i = 0; i < height; i += stepI) {
		for (int j = 0; j < width; j += stepJ) {
			Float2 u(j, i);
			Float2 p = imageOut->transformCenter(u);
			Float2 q = warp(p, warpType);
			//Float2 q = warp(p * s_rep, warpType) * s;
			Float2 v = imageIn->transformLowerLeft(q);

			//v = imageIn->clamp(v);

			Float4 color = imageIn->getPixelBilinear(v);
			
			imageOut->setPixel(u, color);
		}
	}
	warpTimer.Update();
	//printf("Warp time: %ld\n", warpTimer.GetTimeElapsed());
	
	warpTimer.Reset();
	if (stepI > 1)
		imageOut->interpolateAtGrid();
	warpTimer.Update();
	//printf("Up sampling time: %ld\n", warpTimer.GetTimeElapsed());
}

}