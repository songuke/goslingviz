#include "GosLine.h"
#include "GosImage.h"

namespace Gos
{
Line::Line(void)
{
}

Line::~Line(void)
{
}

void Line::draw(Image* image, Float2 src, Float2 dest, Float4 color) {
	int x0 = src.x;
	int y0 = src.y;
	int x1 = dest.x;
	int y1 = dest.y;
	//byte* pixels	= image->getPixels();
	int width		= image->getWidth();
	int height		= image->getHeight();

	/* Another Bresenham by me */	
	// now x0 < x1
	int Dx = x1 - x0; // Dx >= 0 now
	int Dy = y1 - y0;
	bool steep = (abs(Dy) >= abs(Dx));
	if (steep) {
		SWAP(x0, y0);
		SWAP(x1, y1);

		// recompute Dx, Dy after swap
		Dx = x1 - x0;
		Dy = y1 - y0;
	}	

	int xstep = 1;
	if (Dx < 0) {
		xstep = -1;
		Dx = -Dx;
	}

	int ystep = 1;
	if (Dy < 0) {// y1 < y0, decreasing
		ystep = -1;		
		Dy = -Dy; 
	}

	int TwoDy = 2*Dy; 
	int TwoDyTwoDx = TwoDy - 2*Dx; // 2*Dy - 2*Dx
	int E = TwoDy - Dx; //2*Dy - Dx

	int y = y0;
	int xDraw, yDraw;
	
	// FIXME: sometimes infinite loop here!? Cannot use <= or >= as don't know the line goes up or down.
	for (int x = x0; x != x1; x+=xstep) {		
		if (steep) {			
			xDraw = y;
			yDraw = x;
		} else {			
			xDraw = x;
			yDraw = y;
		}

		// plot
		// avoid out of bound when stretching triangle
		//if (xDraw < 0 || xDraw >= width || yDraw < 0 || yDraw >= height) {}
		//else {
			int index = yDraw * width + xDraw;	
			image->setPixel(Float2(xDraw, yDraw), color);
			/*
			pixels[3*index + 0] = 255 * color.x;
			pixels[3*index + 1] = 255 * color.y;
			pixels[3*index + 2] = 255 * color.z;
			*/
		//}

		// next
		if (E > 0) {
			E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
			y = y + ystep;
		} else {
			E += TwoDy; //E += 2*Dy;
		}
	}
}
}
