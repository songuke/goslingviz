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
}

void Warp::render(Image* imageIn, Image* imageOut, Chunk &c) {
	int width = imageOut->getWidth();
	int height = imageOut->getHeight();
	// for every pixel in the output image, find the location in the input image
	// and perform bilinear sampling
	int stepI = 1;
	int stepJ = stepI;
	imageOut->setGrid(stepI, stepJ);

	warpTimer.Reset();

	for (int i = 0; i < height; i += stepI) {
		for (int j = 0; j < width; j += stepJ) {
			Float2 u(j, i);
			Float2 p = imageOut->transformCenter(u);
			Float2 q = warp(p);
			Float2 v = imageIn->transformLowerLeft(q);
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