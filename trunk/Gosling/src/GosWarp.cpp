#include "GosWarp.h"
#include "GosImage.h"
#include "RenzoTimer.h"

using namespace Renzo;
Timer warpTimer;

namespace Gos
{
Warp::Warp(void)
{
	
}

Warp::~Warp(void)
{
}

void Warp::update(int delta) {
	this->delta = delta;
}

void Warp::render(Image* imageIn, Image* imageOut, Chunk &c, int warpType) {
	int width = imageOut->getWidth();
	int height = imageOut->getHeight();
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
	printf("Warp time: %ld\n", warpTimer.GetTimeElapsed());
	
	warpTimer.Reset();
	if (stepI > 1)
		imageOut->interpolateAtGrid();
	warpTimer.Update();
	printf("Up sampling time: %ld\n", warpTimer.GetTimeElapsed());
}

}